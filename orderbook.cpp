#include <iostream>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include "Orderbook.h"
#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"

using namespace std;

bool Orderbook::CanMatch(BuyOrSell buyorsell, Price price) const {
    if (buyorsell == BuyOrSell::Buy) {
        if (asks_.empty()) {
            return false;
        }
        const auto& [bestAsk, _] = *asks_.begin();
        return price >= bestAsk;
    } else {
        if (bids_.empty()) {
            return false;
        }
        const auto& [bestBid, _] = *bids_.begin();
        return price <= bestBid;
    }
}

Trades Orderbook::MatchOrders() {
    Trades trades;
    
    if (orders.empty()) {
        return trades;
    }
    
    try {
        // Matching logic
        while (!bids_.empty() && !asks_.empty()) {
            auto bidIt = bids_.begin();
            auto askIt = asks_.begin();
            
            auto& [bidPrice, bids] = *bidIt;
            auto& [askPrice, asks] = *askIt;
            
            // No match possible if best bid < best ask
            if (bidPrice < askPrice) {
                break;
            }

            // Match orders at these price levels
            while (!bids.empty() && !asks.empty()) {
                auto bidOrderIt = bids.begin();
                auto askOrderIt = asks.begin();
                
                // Get references to the orders
                auto& bidOrder = *bidOrderIt;
                auto& askOrder = *askOrderIt;
                
                // Check for invalid pointers
                if (!bidOrder || !askOrder) {
                    if (!bidOrder) bids.erase(bidOrderIt);
                    if (!askOrder) asks.erase(askOrderIt);
                    
                    // If this emptied a list, clean up the price level
                    if (bids.empty()) bids_.erase(bidPrice);
                    if (asks.empty()) asks_.erase(askPrice);
                    
                    // If either side is now empty, break out of matching
                    if (bids_.empty() || asks_.empty()) break;
                    continue;
                }
                
                // Store IDs early so we still have them after filling
                OrderId bidId = bidOrder->GetOrderId();
                OrderId askId = askOrder->GetOrderId();
                
                // Calculate match quantity
                Quantity quantity = min(bidOrder->GetRemainingQuantity(), askOrder->GetRemainingQuantity());
                
                // Fill orders
                bidOrder->Fill(quantity);
                askOrder->Fill(quantity);
                
                // Record the trade
                trades.push_back(Trade{
                    TradeInfo{ bidId, bidOrder->GetPrice(), quantity },
                    TradeInfo{ askId, askOrder->GetPrice(), quantity }
                });
                
                // Check if orders are now filled 
                if (bidOrder->IsFilled()) {
                    bids.erase(bidOrderIt); // Remove from price level
                    orders.erase(bidId);    // Remove from lookup
                    
                    // If this price level is now empty, remove it
                    if (bids.empty()) {
                        bids_.erase(bidPrice);
                        break; // Need to break as we've invalidated bidIt
                    }
                }
                
                if (askOrder->IsFilled()) {
                    asks.erase(askOrderIt); // Remove from price level
                    orders.erase(askId);    // Remove from lookup
                    
                    // If this price level is now empty, remove it
                    if (asks.empty()) {
                        asks_.erase(askPrice);
                        break; // Need to break as we've invalidated askIt
                    }
                }
            }
            
            // Need to break if we've emptied either side
            if (bids_.empty() || asks_.empty()) break;
        }
    }
    catch (const exception& e) {
        cerr << "Error during matching: " << e.what() << endl;
    }
    
    return trades;
}

Trades Orderbook::AddOrder(OrderPointer order) {
    if (!order) {
        cout << "Ignoring null order" << endl;
        return {};
    }

    auto orderId = order->GetOrderId();
    if (orders.find(orderId) != orders.end()) {
        cout << "Order " << orderId << " already exists" << endl;
        return {};
    }

    if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetBuyOrSell(), order->GetPrice())) {
        cout << "FillAndKill order " << orderId << " would not match, discarding" << endl;
        return {};
    }

    try {
        // Add to appropriate side
        OrderPointers::iterator itr;
        if (order->GetBuyOrSell() == BuyOrSell::Buy) {
            auto& ordersAtPrice = bids_[order->GetPrice()];
            ordersAtPrice.push_back(order);
            itr = prev(ordersAtPrice.end());
        } else {
            auto& ordersAtPrice = asks_[order->GetPrice()];
            ordersAtPrice.push_back(order);
            itr = prev(ordersAtPrice.end());
        }
        
        // Store order in lookup map
        orders.insert({orderId, OrderEntry{order, itr}});
        
        // Try to match orders
        return MatchOrders();
    }
    catch (const exception& e) {
        cerr << "Error adding order: " << e.what() << endl;
        return {};
    }
}

void Orderbook::CancelOrder(OrderId orderId) {
    try {
        // Find the order
        auto it = orders.find(orderId);
        if (it == orders.end()) {
            return;
        }
        
        // Get a copy of the order entry before removing it
        const OrderEntry& entry = it->second;
        if (!entry.order) {
            orders.erase(it);
            return;
        }
        
        // Get copies of all needed data BEFORE erasing
        OrderPointer order = entry.order;
        OrderPointers::iterator location = entry.location;
        BuyOrSell side = order->GetBuyOrSell();
        Price price = order->GetPrice();
        
        // Remove from orders map first
        orders.erase(it);
        
        // Now remove from the appropriate price level
        if (side == BuyOrSell::Buy) {
            auto bidIt = bids_.find(price);
            if (bidIt != bids_.end()) {
                bidIt->second.erase(location);
                
                // Clean up empty price levels
                if (bidIt->second.empty()) {
                    bids_.erase(bidIt);
                }
            }
        } else { // Sell side
            auto askIt = asks_.find(price);
            if (askIt != asks_.end()) {
                askIt->second.erase(location);
                
                // Clean up empty price levels
                if (askIt->second.empty()) {
                    asks_.erase(askIt);
                }
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error canceling order: " << e.what() << endl;
    }
}

Trades Orderbook::MatchOrder(OrderModify modOrder) {
    try {
        // Find the original order
        auto orderId = modOrder.GetOrderId();
        auto it = orders.find(orderId);
        if (it == orders.end()) {
            return {};
        }
        
        // Get the original order's type
        const OrderEntry& entry = it->second;
        if (!entry.order) {
            orders.erase(it);
            return {};
        }
        
        OrderType type = entry.order->GetOrderType();
        
        // Cancel the old order and add the new one
        CancelOrder(orderId);
        return AddOrder(modOrder.ToOrderPointer(type));
    }
    catch (const exception& e) {
        cerr << "Error modifying order: " << e.what() << endl;
        return {};
    }
}

size_t Orderbook::Size() const { 
    return orders.size(); 
}

void Orderbook::ClearAll() {
    bids_.clear();
    asks_.clear();
    orders.clear();
}

OrderPointer Orderbook::FindOrder(OrderId orderid) const {
    auto it = orders.find(orderid);
    if (it != orders.end()) {
        return it->second.order;
    }
    return nullptr;
} 
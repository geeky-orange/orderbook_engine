#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>

#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"
#include "Orderbook.h"

using namespace std;

// Helper function to print order details
void printOrder(const OrderPointer& order) {
    cout << "  ID: " << order->GetOrderId()
         << ", Type: " << (order->GetOrderType() == OrderType::GoodTillCancel ? "GTC" : "FAK")
         << ", Side: " << (order->GetBuyOrSell() == BuyOrSell::Buy ? "Buy" : "Sell")
         << ", Price: " << fixed << setprecision(2) << order->GetPrice()
         << ", Quantity: " << order->GetInitalQuantity()
         << ", Remaining: " << order->GetRemainingQuantity() << endl;
}

// Helper function to print trade details
void printTrade(const Trade& trade) {
    const auto& bidTrade = trade.GetBidTrade();
    const auto& askTrade = trade.GetAskTrade();
    
    cout << "TRADE EXECUTED: " << endl;
    cout << "  Bid Order ID: " << bidTrade.orderid 
         << ", Price: " << fixed << setprecision(2) << bidTrade.price
         << ", Quantity: " << bidTrade.quantity << endl;
    cout << "  Ask Order ID: " << askTrade.orderid 
         << ", Price: " << fixed << setprecision(2) << askTrade.price
         << ", Quantity: " << askTrade.quantity << endl;
    cout << "----------------------" << endl;
}

// Test the Order class functionality
void testOrderClass() {
    cout << "\n===== TESTING ORDER CLASS =====\n" << endl;
    
    // Create an order
    cout << "Creating order..." << endl;
    auto order = make_shared<Order>(OrderType::GoodTillCancel, 1, BuyOrSell::Buy, 100.50, 10);
    printOrder(order);
    
    // Test fill
    cout << "\nFilling 3 units..." << endl;
    order->Fill(3);
    cout << "  Remaining: " << order->GetRemainingQuantity() << endl;
    cout << "  Filled: " << order->GetFilledQuantity() << endl;
    
    // Test IsFilled
    cout << "\nIs order filled? " << (order->IsFilled() ? "Yes" : "No") << endl;
    
    // Test fill remaining
    cout << "Filling remaining 7 units..." << endl;
    order->Fill(7);
    cout << "  Remaining: " << order->GetRemainingQuantity() << endl;
    cout << "  Filled: " << order->GetFilledQuantity() << endl;
    
    // Check if filled now
    cout << "Is order filled? " << (order->IsFilled() ? "Yes" : "No") << endl;
}

// Test the OrderModify class functionality
void testOrderModifyClass() {
    cout << "\n===== TESTING ORDER MODIFY CLASS =====\n" << endl;
    
    // Create an order modify
    OrderModify modOrder(100, BuyOrSell::Buy, 99.99, 5);
    cout << "Created order modify:" << endl;
    cout << "  ID: " << modOrder.GetOrderId() << endl;
    cout << "  Side: " << (modOrder.GetBuyOrSell() == BuyOrSell::Buy ? "Buy" : "Sell") << endl;
    cout << "  Price: " << fixed << setprecision(2) << modOrder.GetPrice() << endl;
    cout << "  Quantity: " << modOrder.GetQuantity() << endl;
    
    // Convert to order pointer
    cout << "\nConverting to order pointer..." << endl;
    auto order = modOrder.ToOrderPointer(OrderType::GoodTillCancel);
    printOrder(order);
}

// Test basic orderbook functionality
void testBasicOrderbook() {
    cout << "\n===== TESTING BASIC ORDERBOOK FUNCTIONALITY =====\n" << endl;
    
    // Create orderbook
    cout << "Creating orderbook..." << endl;
    Orderbook orderbook;
    
    // Test adding orders
    cout << "\nAdding buy order ID: 1 (Price: 100, Qty: 10)" << endl;
    auto order1 = make_shared<Order>(OrderType::GoodTillCancel, 1, BuyOrSell::Buy, 100, 10);
    orderbook.AddOrder(order1);
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Add another order
    cout << "\nAdding buy order ID: 2 (Price: 101, Qty: 5)" << endl;
    auto order2 = make_shared<Order>(OrderType::GoodTillCancel, 2, BuyOrSell::Buy, 101, 5);
    orderbook.AddOrder(order2);
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Cancel an order
    cout << "\nCanceling order ID: 1" << endl;
    orderbook.CancelOrder(1);
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Add back an order with same ID
    cout << "\nAdding buy order ID: 1 (Price: 102, Qty: 7)" << endl;
    auto order3 = make_shared<Order>(OrderType::GoodTillCancel, 1, BuyOrSell::Buy, 102, 7);
    orderbook.AddOrder(order3);
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Modify an order
    cout << "\nModifying order ID: 1 (New Price: 103, New Qty: 8)" << endl;
    OrderModify modOrder(1, BuyOrSell::Buy, 103, 8);
    orderbook.MatchOrder(modOrder);
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Clear the orderbook
    cout << "\nClearing orderbook" << endl;
    orderbook.ClearAll();
    cout << "Orderbook size: " << orderbook.Size() << endl;
}

// Test order matching functionality
void testOrderMatching() {
    cout << "\n===== TESTING ORDER MATCHING =====\n" << endl;
    
    // Create orderbook
    cout << "Creating fresh orderbook..." << endl;
    Orderbook orderbook;
    
    // Add buy orders
    cout << "\nAdding buy orders:" << endl;
    
    cout << "  Buy order ID: 101 (Price: 100.00, Qty: 10)" << endl;
    orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 101, BuyOrSell::Buy, 100.00, 10));
    
    cout << "  Buy order ID: 102 (Price: 101.00, Qty: 5)" << endl;
    orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 102, BuyOrSell::Buy, 101.00, 5));
    
    cout << "  Buy order ID: 103 (Price: 99.00, Qty: 7)" << endl;
    orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 103, BuyOrSell::Buy, 99.00, 7));
    
    cout << "Orderbook size after adding buy orders: " << orderbook.Size() << endl;
    
    // Add matching sell order
    cout << "\nAdding sell order ID: 201 (Price: 100.00, Qty: 3)" << endl;
    cout << "This should match with buy order ID: 102 (highest price)" << endl;
    
    auto trades = orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 201, BuyOrSell::Sell, 100.00, 3));
    
    cout << "Trades executed: " << trades.size() << endl;
    for (const auto& trade : trades) {
        printTrade(trade);
    }
    
    cout << "Orderbook size after matching: " << orderbook.Size() << endl;
    
    // Add another matching sell order
    cout << "\nAdding larger sell order ID: 202 (Price: 99.00, Qty: 15)" << endl;
    cout << "This should match with remaining qty from ID: 102 and ID: 101" << endl;
    
    trades = orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 202, BuyOrSell::Sell, 99.00, 15));
    
    cout << "Trades executed: " << trades.size() << endl;
    for (const auto& trade : trades) {
        printTrade(trade);
    }
    
    cout << "Orderbook size after matching: " << orderbook.Size() << endl;
    
    // Add a FillAndKill order
    cout << "\nAdding FillAndKill buy order ID: 301 (Price: 98.00, Qty: 5)" << endl;
    cout << "This should not match with any sell order and be discarded" << endl;
    
    trades = orderbook.AddOrder(make_shared<Order>(OrderType::FillAndKill, 301, BuyOrSell::Buy, 98.00, 5));
    
    cout << "Trades executed: " << trades.size() << endl;
    cout << "Orderbook size: " << orderbook.Size() << endl;
    
    // Add a matching FillAndKill order
    cout << "\nAdding sell order ID: 203 (Price: 100.00, Qty: 2)" << endl;
    orderbook.AddOrder(make_shared<Order>(OrderType::GoodTillCancel, 203, BuyOrSell::Sell, 100.00, 2));
    
    cout << "\nAdding FillAndKill buy order ID: 302 (Price: 100.00, Qty: 1)" << endl;
    cout << "This should match with sell order ID: 203" << endl;
    
    trades = orderbook.AddOrder(make_shared<Order>(OrderType::FillAndKill, 302, BuyOrSell::Buy, 100.00, 1));
    
    cout << "Trades executed: " << trades.size() << endl;
    for (const auto& trade : trades) {
        printTrade(trade);
    }
    
    cout << "Final orderbook size: " << orderbook.Size() << endl;
}

int main() {
    try {
        cout << "\n======================================================" << endl;
        cout << "          ORDERBOOK ENGINE COMPREHENSIVE TEST          " << endl;
        cout << "======================================================\n" << endl;
        
        // Test Order class
        testOrderClass();
        
        // Test OrderModify class
        testOrderModifyClass();
        
        // Test basic orderbook functionality
        testBasicOrderbook();
        
        // Test order matching functionality
        testOrderMatching();
        
        cout << "\n======================================================" << endl;
        cout << "           ALL TESTS COMPLETED SUCCESSFULLY           " << endl;
        cout << "======================================================\n" << endl;
        
        return 0;
    }
    catch (const exception& e) {
        cerr << "\nERROR: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "\nUNKNOWN ERROR OCCURRED" << endl;
        return 1;
    }
} 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <queue>
#include <limits>
#include <numeric>
#include <algorithm>
#include <functional>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <format>
#include <optional>
#include <list>
#include <tuple>

using namespace std;

enum class OrderType{
    GoodTillCancel,  
    FillAndKill
};

enum class BuyOrSell{
    Buy,
    Sell
};

using Price = long double;
using Quantity = uint32_t;
using OrderId = uint64_t;

struct LevelInfo
{
    Price price;
    Quantity quantity;
    OrderId orderId;
};

using LevelInfos = vector<LevelInfo>;

class OrderbookLevelInfos{
    public:
        OrderbookLevelInfos(const LevelInfo& bids, const LevelInfo& asks)
            : bids{bids}
            , asks{asks}
        { }

        const LevelInfos& GetBids() const {return bids;}
        const LevelInfos& GetAsks() const {return asks;}

    private:
        LevelInfos bids;
        LevelInfos asks;

};

//  Order Objects
/*
- Order Type
- Size
- Filled or not filled quantity

*/

// Order(GoodTillCancel, 123, "buy", 20.0, 12)

class Order{
    public:
        Order(OrderType ordertype, OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity)
        : ordertype {ordertype}
        , orderid {orderid}
        , buyorsell {buyorsell}
        , price {price}
        , initialQuantity {quantity}
        , remainingQuantity {quantity}
        { }

        // private member getters

        OrderId GetOrderId() const { return orderid;}
        BuyOrSell GetBuyOrSell() const { return buyorsell;}
        Price GetPrice() const {return price;}
        OrderType GetOrderType() const {return ordertype;}
        Quantity GetInitalQuantity() const {return initialQuantity;}
        Quantity GetRemainingQuantity() const {return remainingQuantity;}
        Quantity GetFilledQuantity() const { return GetInitalQuantity() - GetRemainingQuantity();}

        // private member setters

        void SetOrderId(OrderId newOrderId){
            orderid = newOrderId;
        }

        void SetRemainingQuantity(Quantity newRemainingQuantity){
            remainingQuantity = newRemainingQuantity;
        }

        // helper functions
        bool IsFilled() const { return GetRemainingQuantity() == 0;}


        void Fill(Quantity quantity){
            if (quantity > GetRemainingQuantity()){
                throw logic_error("Order cannot be filled more than the remaining quantity"+ to_string(GetOrderId()));
            }
            remainingQuantity -= quantity;
        }

    private:
        OrderType ordertype;
        OrderId orderid;
        BuyOrSell buyorsell;
        Price price;
        Quantity remainingQuantity;
        Quantity initialQuantity;



};
using OrderPointer = shared_ptr<Order>;
using OrderPointers = list<OrderPointer>;

// ADD , CANCEL AND Modify the error


class OrderModify
{
private:
    OrderId orderid;
    Price price;
    BuyOrSell buyorsell;
    Quantity quantity;


public:
    OrderModify(OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity)

    : orderid {orderid}
    , price {price}
    , buyorsell { buyorsell}
    , quantity {quantity}
    {}

    OrderId GetOrderId() const {return orderid; }
    Price GetPrice() const {return price;}
    BuyOrSell GetBuyOrSell() const {return buyorsell;}
    Quantity GetQuantity() const {return quantity;}

    OrderPointer ToOrderPointer(OrderType type) const {
        return make_shared<Order>(type,GetOrderId(), GetBuyOrSell(), GetPrice(), GetQuantity());
    }
};


struct TradeInfo
{
    OrderId orderid;
    Price price;
    Quantity quantity;
};


class Trade{
    private:
    TradeInfo bidTrade;
    TradeInfo askTrade;
        
    public:
        Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)

        :  bidTrade {bidTrade}
        , askTrade {askTrade }

        { }

        const TradeInfo& GetBidTrade() const {return bidTrade;}
        const TradeInfo& GetAskTrade() const {return askTrade;}

};

using Trades = vector<Trade>;

class Orderbook{
    private:
        // Using the map to represent bids and ask, bids in accending and ask in decending
        struct OrderEntry
        {
            OrderPointer order{nullptr};
            OrderPointers::iterator location; // point to the order
            
        };

        map<Price,OrderPointers, greater<Price>> bids_;
        map<Price,OrderPointers, less<Price>> asks_;
        unordered_map<OrderId, OrderEntry>orders;

        // fill and kill -> if cant match anything in order book its discarded 
        // non kill fill -> add to the order book and then add 

        bool CanMatch(BuyOrSell buyorsell, Price price) const {
            if (buyorsell == BuyOrSell::Buy){
                if (asks_.empty()){
                    return false;
                }
                const auto& [bestAsk, _] = *asks_.begin();
                return price >= bestAsk;
            }else{
                if (bids_.empty()){
                    return false;
                }
                const auto& [bestBid, _] = *bids_.begin();
                return price <= bestBid;

            };
        }

        Trades MatchOrders(){
            Trades trades;
            trades.reserve(orders.size());

            while (true)
            {
                if (bids_.empty() || asks_.empty()){
                    break;
                }
                auto& [bidPrice, bids] = *bids_.begin();
                auto& [askPrice, asks] = *asks_.begin();
                
                if (bidPrice < askPrice){
                    break;
                }

                while (bids.size() && asks.size())
                {
                    auto& bid = bids.front();
                    auto& ask = asks.front();

                    Quantity quantity = min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());
                    bid->Fill(quantity);
                    ask->Fill(quantity);

                    if (bid->IsFilled()){
                        bids.pop_front();
                        orders.erase(bid->GetOrderId());
                    }

                    if (ask->IsFilled()){
                        asks.pop_front();
                        orders.erase(ask->GetOrderId());
                    }

                    if (bids.empty()){
                        bids_.erase(bidPrice);
                    }

                    if (asks.empty()){
                        asks_.erase(askPrice);
                    }

                    trades.push_back(Trade{
                        TradeInfo{ bid->GetOrderId(), bid->GetPrice(), quantity },
                        TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity }
                        });
            
                }
                

            }

            if (!bids_.empty()){
                auto& [_,bids] = *bids_.begin();
                auto& order = bids.front();
                if (order->GetOrderType() == OrderType::FillAndKill){
                    CancelOrder(order->GetOrderId());
                }

            }

            if (!asks_.empty()){
                auto& [_,asks] = *asks_.begin();
                auto& order = asks.front();
                if (order->GetOrderType() == OrderType::GoodTillCancel){
                    CancelOrder(order->GetOrderId());
                }


            }

            return trades;
            

        }
        
public:

    Trades AddOrder(OrderPointer order){
        if (orders.find(order->GetOrderId()) != orders.end()){
            return { };
        }

        if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetBuyOrSell(), order->GetPrice())){
            return { };
        }

        OrderPointers::iterator itr;

        if (order->GetBuyOrSell() == BuyOrSell::Buy){
            auto& orders = bids_[order->GetPrice()];
            orders.push_back(order);
            itr = next(orders.begin(), orders.size() - 1);
        }else{
            auto& orders = asks_[order->GetPrice()];
            orders.push_back(order);
            itr = next(orders.begin(), orders.size() - 1);
        }

        orders.insert({order->GetOrderId(), OrderEntry{order,itr}});
        return MatchOrders();

    }

    void CancelOrder(OrderId orderid){
        if (orders.find(orderid) == orders.end()){
            return;
        }
        const auto& [order,orderiterator] = orders.at(orderid); // change orderiterator to itr if any problems
        orders.erase(orderid);

        if (order->GetBuyOrSell() == BuyOrSell ::Sell){
            auto price = order->GetPrice();
            auto& orders = asks_.at(price);
            orders.erase(orderiterator);
            if (orders.empty()){
                asks_.erase(price);
            }
        }else{
            auto price = order->GetPrice();
            auto& orders = bids_.at(price);
            orders.erase(orderiterator);
            if (orders.empty()){
                bids_.erase(price);
            }
        }
    }

    Trades MatchOrder(OrderModify order){
        if ( orders.find(order.GetOrderId()) == orders.end()){
            return { };
        }

        const auto& [existingOrder, _] = orders.at(order.GetOrderId());
        CancelOrder(order.GetOrderId());
        return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));
    }

    size_t Size() const { return orders.size(); }

    // OrderbookLevelInfos GetOrderInfos() const {
    //     LevelInfos bidInfos, askInfos;
    //     bidInfos.reserve(orders.size());
    //     askInfos.reserve(orders.size());

    //     auto CreateLevelInfos = [](Price price, const OrderPointers& orders){
    //         return LevelInfo{price,accumulate(orders.begin(), orders.end(), (Quantity)0,
    //         [](size_t runningSum, const OrderPointer& order)
    //         {return runningSum + order->GetRemainingQuantity(); }) };
    //     };

    //     for (const auto& [price,orders] : bids_)
    //         bidInfos.push_back(CreateLevelInfos(price,orders));

    //     for (const auto& [price,orders] : asks_)
    //         askInfos.push_back(CreateLevelInfos(price,orders));

    //     return OrderbookLevelInfos{ bidInfos, askInfos};

    // }
};

int main(){

    // Testing it
    Orderbook orderbook;
    OrderPointer order1 = make_shared<Order>(OrderType::GoodTillCancel, 1, BuyOrSell::Buy, 20.0, 10);
    OrderPointer order2 = make_shared<Order>(OrderType::GoodTillCancel, 2, BuyOrSell::Sell, 20.0, 10);
    OrderPointer order3 = make_shared<Order>(OrderType::GoodTillCancel, 3, BuyOrSell::Buy, 20.0, 10);
    OrderPointer order4 = make_shared<Order>(OrderType::GoodTillCancel, 4, BuyOrSell::Sell, 20.0, 10);
    OrderPointer order5 = make_shared<Order>(OrderType::GoodTillCancel, 5, BuyOrSell::Buy, 20.0, 10);

    Trades trades = orderbook.AddOrder(order1);
    trades = orderbook.AddOrder(order2);
    trades = orderbook.AddOrder(order3);
    trades = orderbook.AddOrder(order4);
    trades = orderbook.AddOrder(order5);

    cout << "Trades: " << trades.size() << endl;

    



}
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

        void Fill(Quantity quantity){
            if (quantity > GetRemainingQuantity()){
                throw logic_error("Order cannot be filled more than the remaining quantity"+ to_string(GetOrderId()));
            }
            else{
                SetRemainingQuantity(remainingQuantity - quantity);
            }
        }

    private:
        OrderType ordertype;
        OrderId orderid;
        BuyOrSell buyorsell;
        Price price;
        Quantity remainingQuantity;
        Quantity initialQuantity;



};

int main() {
    cout << "Hello, World!" << endl;

    return 0;
}

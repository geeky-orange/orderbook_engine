#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"
#include <list>
#include <map>
#include <unordered_map>

using OrderPointers = std::list<OrderPointer>;

class Orderbook {
public:
    Trades AddOrder(OrderPointer order);
    void CancelOrder(OrderId orderid);
    Trades MatchOrder(OrderModify order);
    size_t Size() const;
    void ClearAll();
    
    // Find an order by ID (returns nullptr if not found)
    OrderPointer FindOrder(OrderId orderid) const;

private:
    struct OrderEntry {
        OrderPointer order;
        OrderPointers::iterator location;
    };

    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders;

    bool CanMatch(BuyOrSell buyorsell, Price price) const;
    Trades MatchOrders();
};

#endif // ORDERBOOK_H
#ifndef ORDER_MODIFY_H
#define ORDER_MODIFY_H

#include "Order.h"

class OrderModify {
public:
    OrderModify(OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity);

    OrderId GetOrderId() const;
    Price GetPrice() const;
    BuyOrSell GetBuyOrSell() const;
    Quantity GetQuantity() const;
    OrderPointer ToOrderPointer(OrderType type) const;

    void SetOrderId(OrderId newOrderId);
    

private:
    OrderId orderid;
    Price price;
    BuyOrSell buyorsell;
    Quantity quantity;
};

#endif // ORDER_MODIFY_H
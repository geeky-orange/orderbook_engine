#ifndef ORDER_H
#define ORDER_H

#include <memory>
#include <stdexcept>
#include <string>

enum class OrderType {
    GoodTillCancel,
    FillAndKill
};

enum class BuyOrSell {
    Buy,
    Sell
};

using Price = long double;
using Quantity = uint32_t;
using OrderId = uint64_t;

class Order {
public:
    Order(OrderType ordertype, OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity);

    OrderId GetOrderId() const;
    BuyOrSell GetBuyOrSell() const;
    Price GetPrice() const;
    OrderType GetOrderType() const;
    Quantity GetInitalQuantity() const;
    Quantity GetRemainingQuantity() const;
    Quantity GetFilledQuantity() const;
    bool IsFilled() const;
    void Fill(Quantity quantity);

    void SetOrderId(OrderId newOrderId);
    void SetRemainingQuantity(Quantity newRemainingQuantity);

private:
    OrderType ordertype;
    OrderId orderid;
    BuyOrSell buyorsell;
    Price price;
    Quantity remainingQuantity;
    Quantity initialQuantity;
};

using OrderPointer = std::shared_ptr<Order>;

#endif // ORDER_H
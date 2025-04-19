#include "Order.h"

Order::Order(OrderType ordertype, OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity)
    : ordertype{ordertype}, orderid{orderid}, buyorsell{buyorsell}, price{price}, initialQuantity{quantity}, remainingQuantity{quantity} {}

OrderId Order::GetOrderId() const { return orderid; }
BuyOrSell Order::GetBuyOrSell() const { return buyorsell; }
Price Order::GetPrice() const { return price; }
OrderType Order::GetOrderType() const { return ordertype; }
Quantity Order::GetInitalQuantity() const { return initialQuantity; }
Quantity Order::GetRemainingQuantity() const { return remainingQuantity; }
Quantity Order::GetFilledQuantity() const { return GetInitalQuantity() - GetRemainingQuantity(); }
bool Order::IsFilled() const { return GetRemainingQuantity() == 0; }

void Order::Fill(Quantity quantity) {
    if (quantity > GetRemainingQuantity()) {
        throw std::logic_error("Order cannot be filled more than the remaining quantity" + std::to_string(GetOrderId()));
    }
    remainingQuantity -= quantity;
}

void Order::SetOrderId(OrderId newOrderId) { orderid = newOrderId; }
void Order::SetRemainingQuantity(Quantity newRemainingQuantity) { remainingQuantity = newRemainingQuantity; }
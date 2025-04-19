#include "OrderModify.h"

OrderModify::OrderModify(OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity)
    : orderid(orderid), buyorsell(buyorsell), price(price), quantity(quantity) {}

OrderId OrderModify::GetOrderId() const { return orderid; }
Price OrderModify::GetPrice() const { return price; }
BuyOrSell OrderModify::GetBuyOrSell() const { return buyorsell; }
Quantity OrderModify::GetQuantity() const { return quantity; }

void OrderModify::SetOrderId(OrderId newOrderId) { orderid = newOrderId; }

OrderPointer OrderModify::ToOrderPointer(OrderType type) const {
    return std::make_shared<Order>(type, GetOrderId(), GetBuyOrSell(), GetPrice(), GetQuantity());
} 
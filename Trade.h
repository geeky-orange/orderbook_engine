#ifndef TRADE_H
#define TRADE_H

#include <vector>
#include "Order.h"

struct TradeInfo {
    OrderId orderid;
    Price price;
    Quantity quantity;
};

class Trade {
public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade);

    const TradeInfo& GetBidTrade() const;
    const TradeInfo& GetAskTrade() const;

private:
    TradeInfo bidTrade;
    TradeInfo askTrade;
};

using Trades = std::vector<Trade>;

#endif // TRADE_H
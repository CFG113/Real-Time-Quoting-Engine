#pragma once

#include "TradeInfo.h"

class Trade
{
public:
    Trade(const TradeInfo& tradeInfoOne, const TradeInfo& tradeInfoTwo) : tradeInfoOne_{ tradeInfoOne }, tradeInfoTwo_{ tradeInfoTwo }
    {
        if (tradeInfoOne.priority_ > tradeInfoTwo.priority_)
            throw std::logic_error("Incorrect priority.");
    }

    const TradeInfo& GetFirstTrade() const { return tradeInfoOne_; }
    const TradeInfo& GetSecondTrade() const { return tradeInfoTwo_; }

private:
    TradeInfo tradeInfoOne_;
    TradeInfo tradeInfoTwo_;
};

using Trades = std::vector<Trade>;
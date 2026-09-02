#pragma once

#include "LevelInfo.h"

class OrderbookInfos
{
public:

    OrderbookInfos(const LevelInfos& buys, const LevelInfos& sells)
        : buys_{ buys }, sells_{ sells } { }

    const LevelInfos& GetBuys() const { return buys_; }
    const LevelInfos& GetSells() const { return sells_; }
private:
    LevelInfos buys_;
    LevelInfos sells_;
};
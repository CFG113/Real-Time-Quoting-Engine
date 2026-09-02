#pragma once

#include "Usings.h"

struct TradeInfo
{
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
    Priority priority_;
};
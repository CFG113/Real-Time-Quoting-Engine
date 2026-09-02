#pragma once

#include <map>
#include <unordered_map>

#include "Usings.h"
#include "Order.h"
#include "OrderModify.h"
#include "OrderbookInfos.h"
#include "Trade.h"

class Orderbook
{

public:
    Trades AddOrder(OrderPointer order);
    Trades ModifyOrder(OrderModify order);
    void CancelOrder(OrderId orderId);

    std::size_t Size() const;
    OrderbookInfos GetOrderInfos() const;

private:
    bool CanMatchOrder(Side side, Price price) const;
    Trades MatchOrders();
    struct OrderEntry
    {
        OrderPointer order_;
        OrderPointerList::iterator location_;
    };

    std::map<Price, OrderPointerList, std::greater<Price>> buyOrders_;
    std::map<Price, OrderPointerList, std::less<Price>> sellOrders_;
    std::unordered_map<OrderId, OrderEntry> orders_;
};
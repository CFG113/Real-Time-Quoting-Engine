#include "Orderbook.h"

#include <numeric>

Trades Orderbook::AddOrder(OrderPointer order)
{
    if (orders_.count(order->GetId()))
        return { };

    if (order->GetType() == OrderType::InsertOrCancel && !CanMatchOrder(order->GetSide(), order->GetPrice()))
        return { };

    OrderPointerList::iterator iterator;

    if (order->GetSide() == Side::Buy)
    {
        auto& orders = buyOrders_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }
    else
    {
        auto& orders = sellOrders_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }

    orders_.insert({ order->GetId(), {order, iterator} });
    return MatchOrders();
}


Trades Orderbook::ModifyOrder(OrderModify order)
{
    if (!orders_.count(order.GetId()))
        return { };

    const auto [currentOrder, _] = orders_.at(order.GetId());

    CancelOrder(order.GetId());
    const auto& trades = AddOrder(order.ToOrderPointer(currentOrder->GetType()));
    return trades;
}

void Orderbook::CancelOrder(OrderId orderId)
{
    if (!orders_.count(orderId))
        return;

    const auto [order, orderIterator] = orders_.at(orderId);
    orders_.erase(orderId);

    auto IsOrder = [orderId](const OrderPointer& order) { return order->GetId() == orderId; };

    if (order->GetSide() == Side::Sell)
    {
        auto price = order->GetPrice();
        auto& orders = sellOrders_.at(price);
        orders.erase(orderIterator);
        if (orders.empty())
            sellOrders_.erase(price);
    }
    else
    {
        auto price = order->GetPrice();
        auto& orders = buyOrders_.at(price);
        orders.erase(orderIterator);
        if (orders.empty())
            buyOrders_.erase(price);
    }
}

std::size_t Orderbook::Size() const
{
    return orders_.size(); 
}

OrderbookInfos Orderbook::GetOrderInfos() const
{
    LevelInfos buyOrderInfos, sellOrderInfos;
    buyOrderInfos.reserve(orders_.size());
    sellOrderInfos.reserve(orders_.size());

    auto CreateLevelInfo = [](Price price, const OrderPointerList& orders)
    {
        return LevelInfo{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0, 
            [](std::size_t runningSum, const OrderPointer& order) 
            { return runningSum + order->GetRemainingQuantity(); }) };
    };

    for (auto it = buyOrders_.begin(); it != buyOrders_.end(); it++)
    {
        const auto& [price, orders] = *it;
        buyOrderInfos.push_back(CreateLevelInfo(price, orders));
    }
    
    for (auto it = sellOrders_.rbegin(); it != sellOrders_.rend(); it++)
    {
        const auto& [price, orders] = *it;
        sellOrderInfos.push_back(CreateLevelInfo(price, orders));
    }

    return { buyOrderInfos, sellOrderInfos };
}

bool Orderbook::CanMatchOrder(Side side, Price price) const
{
    if (side == Side::Sell)
    {
        if (!buyOrders_.size())
            return false;

        auto bestBidPrice = (*buyOrders_.begin()).first;
        return price <= bestBidPrice;
    }
    else
    {
        if (!sellOrders_.size())
            return false;

        auto bestAskPrice = (*sellOrders_.begin()).first;
        return price >= bestAskPrice;
    }
}

Trades Orderbook::MatchOrders()
{
    Trades trades;
    trades.reserve(orders_.size());

    while (true)
    {
        if (!buyOrders_.size() || !sellOrders_.size())
            break;

        auto& [buyPrice, buyOrders] = *buyOrders_.begin();
        auto& [sellPrice, sellOrders] = *sellOrders_.begin();

        if (buyPrice < sellPrice)
            break;

        while (buyOrders.size() && sellOrders.size())
        {
            auto& buyOrder = buyOrders.front();
            auto& sellOrder = sellOrders.front();

            Quantity tradeQuantity = std::min(buyOrder->GetRemainingQuantity(), sellOrder->GetRemainingQuantity());

            buyOrder->Fill(tradeQuantity);
            sellOrder->Fill(tradeQuantity);

            if (!buyOrder->GetRemainingQuantity())
            {
                buyOrders.pop_front();
                orders_.erase(buyOrder->GetId());
            }
            if (!sellOrder->GetRemainingQuantity())
            {
                sellOrders.pop_front();
                orders_.erase(sellOrder->GetId());
            }

            if (buyOrders.empty())
                buyOrders_.erase(buyPrice);
            if (sellOrders.empty())
                sellOrders_.erase(sellPrice);

            const OrderPointer& firstOrder = buyOrder->GetPriority() < sellOrder->GetPriority() ? buyOrder : sellOrder;
            const OrderPointer& secondOrder = firstOrder->GetId() == buyOrder->GetId() ? sellOrder : buyOrder;

            trades.emplace_back(
                TradeInfo{ firstOrder->GetId(), firstOrder->GetPrice(), tradeQuantity, firstOrder->GetPriority() },
                TradeInfo{ secondOrder->GetId(), secondOrder->GetPrice(), tradeQuantity, secondOrder->GetPriority() });
        }
    }

    if (!buyOrders_.empty())
    {
        auto& [_, buyOrders] = *buyOrders_.begin();
        OrderPointer firstOrder = buyOrders.front();
        if (firstOrder->GetType() == OrderType::InsertOrCancel)
        {
            CancelOrder(firstOrder->GetId());
        }
    }

    if (!sellOrders_.empty())
    {
        auto& [_, sellOrders] = *sellOrders_.begin();
        auto firstOrder = sellOrders.front();
        if (firstOrder->GetType() == OrderType::InsertOrCancel)
        {
            CancelOrder(firstOrder->GetId());
        }
    }

    return trades;
}


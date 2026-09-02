#pragma once

#include <list>
#include <exception>
#include <format>

#include "OrderType.h"
#include "Side.h"
#include "Usings.h"
#include "Constants.h"

class Order
{
public:

    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity) :
        orderType_{ orderType }, orderId_{ orderId }, price_{ price }, side_{ side }, initialQuantity_{ quantity }, remainingQuantity_{ quantity }
    { }

    OrderId GetId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }
    void Fill(Quantity quantity)
    {
        if (quantity > remainingQuantity_)
            throw std::logic_error("Cannot fill an order for more than its quantity.");

        remainingQuantity_ -= quantity;
    }
    Price GetPrice() const { return price_; }
    OrderType GetType() const { return orderType_; }
    Priority GetPriority() const { return priority_; }

private:

    OrderType orderType_;
    OrderId orderId_;
    Price price_;
    Side side_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
    Priority priority_{ OrderPriority++ }; // Unused

    static inline Priority OrderPriority = 0;
    // static Priority OrderPriority;
};

// Priority Order::OrderPriority = 0;

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::vector<OrderPointer>;
using OrderPointerList = std::list<OrderPointer>;
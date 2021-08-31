#pragma once

#include <variant>

using UniqId = int;

// strong types for variant
struct Quantity
{
    int value{0};
};

struct Price
{
    int value{0};
};

auto price_hash = [](Price const &p)
{
    return std::hash<int>()(p.value);
};

struct Order
{
    UniqId id;
    Price price;
    Quantity quantity;
};

using OrderPtr = std::shared_ptr<Order>;

struct OrderAction
{
};

struct OrderCreate : OrderAction
{
    Order order;
    OrderCreate() = delete;
    explicit OrderCreate(Order o)
        : order(o){};
};

struct OrderModify : OrderAction
{
    UniqId id;
    std::variant<Price, Quantity> change;

    OrderModify() = delete;
    explicit OrderModify(UniqId i, std::variant<Price, Quantity> c)
        : id(i), change(c){};
};

struct OrderRemove : OrderAction
{
    UniqId id;

    OrderRemove() = delete;
    explicit OrderRemove(UniqId i) : id(i) {}
};
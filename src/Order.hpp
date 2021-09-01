#pragma once

#include <variant>

namespace order
{
    using UniqId = int;

    // strong types for variant
    struct Quantity
    {
        int value{0};

        auto operator<=>(auto const &rhs) const { return value <=> rhs.value; }
        auto operator==(auto const &rhs) const { return value == rhs.value; }
    };

    struct Price
    {
        int value{0};

        auto operator<=>(auto const &rhs) const { return value <=> rhs.value; }
        auto operator==(auto const &rhs) const { return value == rhs.value; }
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
        Order(UniqId i, Price p, Quantity q)
            : id(i), price(p), quantity(q){};
    };

    using OrderPtr = std::shared_ptr<Order>;

    struct OrderAction
    {
    };

    struct OrderCreate : OrderAction
    {
        OrderPtr order;
        OrderCreate() = delete;
        explicit OrderCreate(OrderPtr o)
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

} // order
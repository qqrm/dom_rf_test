#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <shared_mutex>
#include <string>
#include <ranges>
#include <functional>
#include <algorithm>
#include <numeric>
#include <set>

#include "Order.hpp"

using namespace order;

class OrderBook
{
private:
    const std::string instrument_;
    std::unordered_map<UniqId, OrderPtr> orders_;
    // std::unordered_map<Price, std::set<OrderPtr>, decltype(price_hash)> orders_by_prices_;

    std::unordered_map<Price, Quantity, decltype(price_hash)> quantitys_;

public:
    OrderBook() = delete;
    explicit OrderBook(std::string instrument)
        : instrument_(instrument){};

    ~OrderBook() = default;

    auto GetInstrument() const -> std::string
    {
        return instrument_;
    };

    auto Add(OrderPtr order_ptr) -> void
    {
        orders_.insert({order_ptr->id, order_ptr});
        quantitys_[order_ptr->price].value += order_ptr->quantity.value;
    };

    auto Modify(OrderModify const &order_mod) -> void
    {
        if (orders_.contains(order_mod.id))
        {
            auto order = orders_[order_mod.id];

            if (0 == order_mod.change.index())
            {
                // TODO: add math ops for Price and Quantity
                quantitys_[order->price].value -= order->quantity.value;
                order->price = std::get<Price>(order_mod.change);
                quantitys_[order->price].value += order->quantity.value;
            }
            else
            {
                auto new_q{std::get<Quantity>(order_mod.change)};
                auto diff{new_q.value - order->quantity.value};
                quantitys_[order->price].value += diff;
                order->quantity.value = new_q.value;
            }
        }
    };

    auto Remove(UniqId id) -> void
    {
        if (orders_.contains(id))
        {
            auto order_ptr{orders_[id]};
            auto price{order_ptr->price};
            quantitys_[price].value -= order_ptr->quantity.value;

            // for buy/sell books should be changed, but now it's ok
            if (0 == quantitys_[price].value)
            {
                quantitys_.erase(price);
            }

            orders_.erase(id);
        }
    };

    auto OrdersCount() -> size_t
    {
        return orders_.size();
    };

    auto GetQuantity(Price price) const -> std::optional<Quantity>
    {
        if (quantitys_.contains(price))
        {
            return {quantitys_.find(price)->second};
        }

        return {};
    }
};

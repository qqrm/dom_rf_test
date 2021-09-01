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

class OrderBook
{
private:
    const std::string instrument_;
    std::unordered_map<UniqId, OrderPtr> orders_;
    std::unordered_map<Price, std::set<OrderPtr>, decltype(price_hash)> orders_by_prices_;

    auto calcQuantity(const Price price) const -> Quantity
    {
        if (orders_by_prices_.contains(price))
        {
            auto const orders{orders_by_prices_.find(price)->second};
            return {std::accumulate(orders.begin(),
                                    orders.end(),
                                    0, [](int acc, OrderPtr const &order)
                                    { return acc + order->quantity.value; })};
        }

        return {};
    }

public:
    OrderBook() = delete;
    explicit OrderBook(std::string instrument)
        : instrument_(instrument){};

    ~OrderBook() = default;

    auto GetInstrument() const -> std::string { return instrument_; };

    auto Add(OrderPtr order_ptr) -> void
    {
        orders_.insert({order_ptr->id, order_ptr});
        orders_by_prices_[order_ptr->price].insert(order_ptr);
    };

    auto Remove(UniqId id) -> void
    {
        if (orders_.contains(id))
        {
            auto order_ptr{orders_[id]};
            orders_.erase(id);

            auto price{order_ptr->price};

            if (orders_by_prices_[price].size() == 1)
            {
                orders_by_prices_.erase(price);
                return;
            }

            orders_by_prices_[price].erase(order_ptr);
        }
    };

    auto OrdersCount() -> size_t
    {
        return orders_.size();
    };

    auto GetQuantity(const Price price) const -> Quantity
    {
        // posible check cashed values here if nedeed
        return calcQuantity(price);
    }
};

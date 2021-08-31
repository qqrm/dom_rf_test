#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <shared_mutex>
#include <string>
#include <ranges>
#include <functional>
#include <numeric>

#include "Order.hpp"

class OrderBook
{
private:
    const std::string instrument_;
    std::unordered_map<UniqId, OrderPtr> orders_;

    std::unordered_map<Price, OrderPtr, decltype(price_hash)> orders_by_prices_;

    auto calcQuantity(const Price price) const -> Quantity
    {
        //     auto r = std::ranges::filter_view(orders_by_prices_.begin(), [price](auto const &p)
        //                                       { return p.first == price; });

        //   | std::ranges::values_views;

        // auto sum = std::reduce( , 0);

        return Quantity{};
    }

public:
    OrderBook() = delete;
    explicit OrderBook(std::string instrument)
        : instrument_(instrument){};

    ~OrderBook() = default;

    auto GetInstrument() const -> std::string { return instrument_; };

    auto Add(UniqId order_id) -> void{
        // orders_.insert(order_id);
    };

    auto Remove(UniqId order_id) -> void
    {
        // orders_.erase(order_id);
    }

    auto GetQuantity(const Price price) const -> Quantity
    {
        // posible check cashed values here if nedeed
        return calcQuantity(price);
    }
};

#pragma once

// #include <functional>
#include <string>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <string_view>
#include <tuple>
#include <iterator>

#include "Order.hpp"

using namespace order;

using Action = std::tuple<std::string, std::shared_ptr<OrderAction>>;

class OrdersGenerator
{
    struct OrdersIdCounters
    {
        int current_create_id{0};
        int current_remove_id{0}; // for manual tests
        int current_modify_id{0};
    };

    std::map<std::string_view, OrdersIdCounters> counters_;

    std::set<std::string> instruments_;

    auto getRandomInstr() -> std::string
    {
        size_t n{rand() % instruments_.size()};
        auto it{instruments_.begin()};
        std::advance(it, n);
        return *it;
    }

    auto getModId(std::string_view instr) -> int
    {
        auto mod_id{counters_[instr].current_modify_id++};

        auto cre_id{counters_[instr].current_create_id};
        auto rem_id{counters_[instr].current_remove_id};

        if (mod_id < cre_id && mod_id <= rem_id)
        {
            return mod_id;
        }
        else
        {
            return cre_id;
        }

        throw "shit";
    }

    auto getRemoveId(std::string_view instr) -> int
    {
        auto rem_id{counters_[instr].current_remove_id++};

        auto mod_id{counters_[instr].current_modify_id};
        auto cre_id{counters_[instr].current_create_id};

        if (rem_id < cre_id && rem_id <= mod_id)
        {
            return rem_id;
        }

        throw "shit";
    }

    auto getCreateId(std::string_view instr) -> int
    {
        auto cre_id{counters_[instr].current_create_id++};

        auto mod_id{counters_[instr].current_modify_id};
        auto rem_id{counters_[instr].current_remove_id};

        if (cre_id < rem_id && cre_id < mod_id)
        {
            throw "shit";
        }

        return cre_id;
    }

public:
    OrdersGenerator(std::set<std::string> const &instruments)
        : instruments_(instruments)
    {
        for (auto const &inst : instruments_)
        {
            counters_.insert(std::make_pair(inst, OrdersIdCounters{}));
        }
    }

    auto GenerateCreate(std::string_view instr) -> std::unique_ptr<OrderAction>
    {
        auto cr_id{getCreateId(instr)};

        auto order_ptr{std::make_shared<Order>(cr_id,
                                               Price{rand() % 99 + 1},
                                               Quantity{rand() % 99 + 1})};

        return std::make_unique<OrderCreate>(order_ptr);
    }

    auto GenerateModify(std::string_view instr) -> std::unique_ptr<OrderModify>
    {
        auto mod_id{getModId(instr)};

        if (mod_id < 50)
        {
            return nullptr;
        }

        std::variant<Price, Quantity> c;

        if (rand() % 1)
        {
            c = Price{rand() % 999 + 1};
        }
        else
        {
            c = Quantity{rand() % 99 + 1};
        }

        return std::make_unique<OrderModify>(
            mod_id,
            c);
    }

    auto GenerateRemove(std::string_view instr) -> std::unique_ptr<OrderRemove>
    {
        auto rem_id{getRemoveId(instr)};
        if (rem_id < 20)
        {
            return nullptr;
        }

        return std::make_unique<OrderRemove>(
            rem_id);
    }

    auto GenerateAction() -> Action
    {
        static int create_counter;
        static int modify_counter;
        static int remove_counter;

        auto instr{getRandomInstr()};

        if (create_counter < 100)
        {
            create_counter++;
            return {instr, GenerateCreate(instr)};
        }

        auto rnd_val{rand() % 8};
        switch (rnd_val)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            create_counter++;
            return {instr, GenerateCreate(instr)};
        case 5:
        case 6:
        case 7:
            modify_counter++;
            return {instr, GenerateModify(instr)};
        case 8:
            remove_counter++;
            return {instr, GenerateRemove(instr)};
        default:
            throw "error";
        }
    };
};

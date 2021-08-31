#pragma once
#include "OrderBook.hpp"

#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <initializer_list>
#include <iostream>
#include <set>

namespace book_holder
{

    using BookPtr = std::shared_ptr<OrderBook>;
    using BookSet = std::set<std::shared_ptr<OrderBook>>;
    using BookMapById = std::unordered_map<int, std::shared_ptr<OrderBook>>;
    using BookIdByInstr = std::unordered_map<std::string, std::set<int>>;
    using BookId = int;

    class BookHolder
    {
        BookSet books_;
        BookMapById books_id_;
        BookIdByInstr books_by_instr_;
        int id{0};
        std::shared_mutex m_;

    public:
        BookHolder() = default;

        BookHolder(BookSet books)
            : books_(books)
        {
            for (auto book : books_)
            {
                auto cur_id = id++;
                std::cout << "instr: " << book->GetInstrument() << " id: " << cur_id;
                books_id_.insert({cur_id, book});
                books_by_instr_[book->GetInstrument()].insert(cur_id);
            }
        };

        auto GetInstruments() const -> std::set<std::string>
        {
            auto r = std::views::keys(books_by_instr_);
            return {r.begin(), r.end()};
        }

        auto GetCount() const -> size_t
        {
            return books_id_.size();
        }

        auto AddBook(BookPtr book_ptr) -> BookId
        {
            std::string const instr = book_ptr->GetInstrument();

            books_.insert(book_ptr);

            auto cur_id = id++;
            books_id_.insert({cur_id, book_ptr});
            books_by_instr_[book_ptr->GetInstrument()].insert(cur_id);

            return cur_id;
        }

        auto RemoveBook(int book_id) -> void
        {
            if (books_id_.contains(book_id))
            {
                auto book_ptr = books_id_[book_id];

                books_by_instr_[book_ptr->GetInstrument()].erase(book_id);
                books_id_.erase(book_id);
                books_.erase(book_ptr);
            }
        }

        auto AddOrder(std::string instrument, Order order) -> void
        {
        }
    };

    auto BookSetFactory(std::vector<std::string> const &instruments) -> BookSet
    {
        BookSet book_set;

        for (auto const instrument : instruments)
        {
            book_set.insert(std::make_shared<OrderBook>(instrument));
        }

        return book_set;
    }

} // namespace book_holder
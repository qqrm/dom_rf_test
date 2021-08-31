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
    using BookId = int;
    using BookPtr = std::shared_ptr<OrderBook>;
    using BookSet = std::set<std::shared_ptr<OrderBook>>;
    using BookMapById = std::unordered_map<BookId, std::shared_ptr<OrderBook>>;
    using BookIdByInstr = std::unordered_map<std::string, std::set<BookId>>;

    class BookHolder
    {
        BookSet books_;
        BookMapById books_id_;
        BookIdByInstr books_by_instr_;
        int id{0};
        mutable std::shared_mutex m_;

    public:
        BookHolder() = default;
        // BookHolder(const BookHolder &) = default;

        BookHolder(BookSet books)
            : books_(books)
        {
            for (auto book : books_)
            {
                auto cur_id = id++;
                books_id_.insert({cur_id, book});
                books_by_instr_[book->GetInstrument()].insert(cur_id);
            }
        };

        auto GetInstruments() const -> std::set<std::string>
        {
            std::shared_lock lock(m_);
            auto r = std::views::keys(books_by_instr_);
            return {r.begin(), r.end()};
        }

        auto GetCount() const -> size_t
        {
            std::shared_lock lock(m_);
            return books_id_.size();
        }

        auto GetIds() const -> std::set<BookId>
        {
            std::shared_lock lock(m_);

            auto r = std::views::keys(books_id_);
            return {r.begin(), r.end()};
        }

        auto AddBook(BookPtr book_ptr) -> BookId
        {
            std::unique_lock lock(m_);

            std::string const instr = book_ptr->GetInstrument();

            books_.insert(book_ptr);

            auto cur_id = id++;
            books_id_.insert({cur_id, book_ptr});
            books_by_instr_[book_ptr->GetInstrument()].insert(cur_id);

            return cur_id;
        }

        auto RemoveBook(int book_id) -> void
        {
            std::unique_lock lock(m_);
            if (books_id_.contains(book_id))
            {
                auto book_ptr = books_id_[book_id];
                auto instr = book_ptr->GetInstrument();

                books_by_instr_[instr].erase(book_id);
                if (books_by_instr_[instr].empty())
                {
                    books_by_instr_.erase(instr);
                }

                books_id_.erase(book_id);
                books_.erase(book_ptr);
            }
        }

        auto AddOrder(std::string instrument, Order order) -> void
        {
            std::unique_lock lock(m_); // not so good
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
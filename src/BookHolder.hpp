#pragma once
#include "OrderBook.hpp"

#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <initializer_list>
#include <iostream>
#include <set>
#include <mutex>
#include <atomic>

namespace book_holder
{
    using BookPtr = std::shared_ptr<OrderBook>;
    using BookId = int;

    using BookSet = std::set<std::shared_ptr<OrderBook>>;
    using BookMapById = std::vector<std::shared_ptr<OrderBook>>;
    using BookIdByInstr = std::unordered_map<std::string, std::set<BookId>>;

    class BookHolder
    {
        // BookSet books_;
        std::set<int> ids_;
        BookMapById books_id_{30}; // resize checks needs
        BookIdByInstr books_by_instr_;
        std::atomic<int> id{0};
        std::atomic<int> count{0};

        mutable std::shared_mutex mtx_books_id_;
        mutable std::shared_mutex mtx_books_instr_;

    public:
        BookHolder() = default;

        BookHolder(BookSet const &books)
        {
            for (auto book : books)
            {
                auto cur_id{id++};
                books_id_[id] = book;
                books_by_instr_[book->GetInstrument()].insert(cur_id);
                ids_.insert(cur_id);
            }
            count = books.size();
        };

        auto GetInstruments() const -> std::set<std::string>
        {
            std::shared_lock lock(mtx_books_instr_);
            auto r{std::views::keys(books_by_instr_)};
            return {r.begin(), r.end()};
        }

        auto GetCount() const -> size_t
        {
            return count;
        }

        auto GetIdsByInst() const -> BookIdByInstr
        {
            std::shared_lock lock(mtx_books_instr_);
            return books_by_instr_;
        }

        auto AddBook(BookPtr book_ptr) -> BookId
        {
            std::scoped_lock lock(mtx_books_id_, mtx_books_instr_);

            std::string const instr{book_ptr->GetInstrument()};

            auto cur_id{id++};
            ids_.insert(cur_id);
            books_id_[cur_id] = book_ptr;
            books_by_instr_[book_ptr->GetInstrument()].insert(cur_id);

            count++;

            return cur_id;
        }

        auto RemoveBook(int book_id) -> void
        {
            std::scoped_lock lock(mtx_books_id_, mtx_books_instr_);
            if (books_id_[book_id])
            {
                auto book_ptr{books_id_[book_id]};
                auto instr{book_ptr->GetInstrument()};

                books_by_instr_[instr].erase(book_id);
                if (books_by_instr_[instr].empty())
                {
                    books_by_instr_.erase(instr);
                }

                books_id_[book_id] = nullptr;
                ids_.erase(book_id);
                count--;
            }
        }

        auto AddOrder(std::string &instrument, OrderPtr order) -> void
        {
            std::scoped_lock lock(mtx_books_id_, mtx_books_instr_);

            if (books_by_instr_.contains(instrument))
            {
                for (auto book_id : books_by_instr_[instrument])
                {
                    books_id_[book_id]->Add(order);
                }
            }
        }

        auto ModifyOrder(std::string &instrument, OrderModify order) -> void
        {
            std::scoped_lock lock(mtx_books_id_, mtx_books_instr_);

            if (books_by_instr_.contains(instrument))
            {
                for (auto book_id : books_by_instr_[instrument])
                {
                    books_id_[book_id]->Modify(order);
                }
            }
        }

        auto GetQuantity(BookId id, const Price price) const -> std::optional<Quantity>
        {
            if (books_id_[id])
            {
                return {books_id_[id]->GetQuantity(price)};
            }

            return {};
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
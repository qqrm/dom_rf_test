#include "include/acutest.h"
#include "../../src/BookHolder.hpp"

#include <exception>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

using namespace book_holder;

void book_holder_init_tests()
{
    {
        BookHolder book_holder(BookSetFactory({"aaa", "bbb", "ccc"}));

        assert(3 == book_holder.GetCount());
        assert(std::set<std::string>({"aaa", "bbb", "ccc"}) == book_holder.GetInstruments());
    }

    {
        BookHolder book_holder;
        assert(0 == book_holder.GetCount());

        book_holder.AddBook(std::make_shared<OrderBook>("bbb"));
        assert(1 == book_holder.GetCount());
        assert(std::set<std::string>{"bbb"} == book_holder.GetInstruments());

        book_holder.AddBook(std::make_shared<OrderBook>("bbb"));
        assert(2 == book_holder.GetCount());
        assert(std::set<std::string>{"bbb"} == book_holder.GetInstruments());

        book_holder.AddBook(std::make_shared<OrderBook>("aaa"));
        assert(3 == book_holder.GetCount());
        assert(std::set<std::string>({"bbb", "aaa"}) == book_holder.GetInstruments());

        auto id = book_holder.AddBook(std::make_shared<OrderBook>("zzz"));
        assert(4 == book_holder.GetCount());
        assert(std::set<std::string>({"bbb", "aaa", "zzz"}) == book_holder.GetInstruments());

        book_holder.RemoveBook(id);
        assert(3 == book_holder.GetCount());
        assert(std::set<std::string>({"bbb", "aaa"}) == book_holder.GetInstruments());
    }
}

TEST_LIST = {

    {"BookHolder int tests", book_holder_init_tests},


    {NULL, NULL}};
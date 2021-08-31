#include "include/acutest.h"
#include "../../src/BookHolder.hpp"
#include "../../src/OrdersGenerator.hpp"

#include <exception>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

using namespace book_holder;

auto print_ids = [](auto ids)
{
    std::cout << "\nid's: ";
    for (auto id : ids)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";
};

void book_holder_init_tests()
{

    {
        BookHolder bh(BookSetFactory({"aaa", "bbb", "ccc"}));

        assert(3 == bh.GetCount());
        assert(std::set<std::string>({"aaa", "bbb", "ccc"}) == bh.GetInstruments());
    }

    {
        BookHolder bh;
        assert(0 == bh.GetCount());

        bh.AddBook(std::make_shared<OrderBook>("bbb"));
        assert(1 == bh.GetCount());
        assert(std::set<std::string>{"bbb"} == bh.GetInstruments());

        bh.AddBook(std::make_shared<OrderBook>("bbb"));
        assert(2 == bh.GetCount());
        assert(std::set<std::string>{"bbb"} == bh.GetInstruments());

        bh.AddBook(std::make_shared<OrderBook>("aaa"));
        assert(3 == bh.GetCount());
        assert(std::set<std::string>({"bbb", "aaa"}) == bh.GetInstruments());

        auto id = bh.AddBook(std::make_shared<OrderBook>("zzz"));
        assert(4 == bh.GetCount());
        assert(std::set<std::string>({"bbb", "aaa", "zzz"}) == bh.GetInstruments());

        bh.RemoveBook(id);
        assert(3 == bh.GetCount());
        assert(std::set<std::string>({"bbb", "aaa"}) == bh.GetInstruments());

        bh.RemoveBook(0);
        assert(2 == bh.GetCount());
        assert(std::set<std::string>({"bbb", "aaa"}) == bh.GetInstruments());

        bh.RemoveBook(1);
        assert(1 == bh.GetCount());
        assert(std::set<std::string>({"aaa"}) == bh.GetInstruments());
    }
}

void order_book_tests()
{
    OrderBook book("aaa");
    assert(std::string{"aaa"} == book.GetInstrument());

    auto order_ptr = std::make_shared<Order>(111,
                                             Price{333},
                                             Quantity{555});

    assert(0 == book.OrdersCount());
    assert(Quantity{} == book.GetQuantity(Price{333}));

    book.Add(order_ptr);
    assert(1 == book.OrdersCount());
    assert(Quantity{555} == book.GetQuantity(Price{333}));

    book.Add(std::make_shared<Order>(222,
                                     Price{333},
                                     Quantity{222}));
    assert(2 == book.OrdersCount());
    assert(Quantity{555 + 222} == book.GetQuantity(Price{333}));

    book.Remove(111);
    assert(1 == book.OrdersCount());

    assert(Quantity{222} == book.GetQuantity(Price{333}));

    book.Add(std::make_shared<Order>(333,
                                     Price{444},
                                     Quantity{222}));
    assert(2 == book.OrdersCount());
    assert(Quantity{222} == book.GetQuantity(Price{333}));
    assert(Quantity{222} == book.GetQuantity(Price{444}));




}

void order_gen_tests()
{
    BookHolder bh(BookSetFactory({"aaa", "bbb", "ccc"}));

    auto orders_gen = OrdersGenerator(bh.GetInstruments());
}

TEST_LIST = {

    {"BookHolder int tests", book_holder_init_tests},
    {"OrderBook tests", order_book_tests},

    // {"OrderGen tests", order_gen_tests},

    {NULL, NULL}};
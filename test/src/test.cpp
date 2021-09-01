#include "include/acutest.h"
#include "../../src/BookHolder.hpp"
#include "../../src/OrdersGenerator.hpp"

#include <exception>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

using namespace book_holder;

auto print_ids = [](auto books_by_instr)
{
    std::cout << "\n";
    for (auto const &[instument, ids] : books_by_instr)
    {
        std::cout << "instument: " << instument << " id's: ";
        for (auto const id : ids)
        {
            std::cout << id << " ";
        }
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

        auto id{bh.AddBook(std::make_shared<OrderBook>("zzz"))};
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

    auto order_ptr{std::make_shared<Order>(111,
                                           Price{333},
                                           Quantity{555})};

    assert(0 == book.OrdersCount());
    assert(false == book.GetQuantity(Price{333}).has_value());

    book.Add(order_ptr);
    assert(1 == book.OrdersCount());
    assert(Quantity{555} == book.GetQuantity(Price{333}).value());

    book.Add(std::make_shared<Order>(222,
                                     Price{333},
                                     Quantity{222}));
    assert(2 == book.OrdersCount());
    assert(Quantity{555 + 222} == book.GetQuantity(Price{333}).value());

    book.Remove(111);
    assert(1 == book.OrdersCount());

    assert(Quantity{222} == book.GetQuantity(Price{333}).value());

    book.Add(std::make_shared<Order>(333,
                                     Price{444},
                                     Quantity{222}));
    assert(2 == book.OrdersCount());
    assert(Quantity{222} == book.GetQuantity(Price{333}).value());
    assert(Quantity{222} == book.GetQuantity(Price{444}).value());
}

void book_holder_add_order_tests()
{
    BookHolder bh;

    {
        std::string instr = "bbb";
        BookPtr book_ptr = std::make_shared<OrderBook>(instr);

        bh.AddBook(book_ptr);

        auto order_ptr{std::make_shared<Order>(111,
                                               Price{222},
                                               Quantity{333})};

        bh.AddOrder(instr, order_ptr);

        // print_ids(bh.GetIdsByInst());

        assert(book_ptr->GetQuantity(Price{222}).value() == bh.GetQuantity(0, Price{222}).value());

        auto order_ptr1{std::make_shared<Order>(222,
                                                Price{333},
                                                Quantity{444})};

        bh.AddOrder(instr, order_ptr1);

        assert(book_ptr->GetQuantity(Price{222}).value() == bh.GetQuantity(0, Price{222}).value());
        assert(book_ptr->GetQuantity(Price{333}).value() == bh.GetQuantity(0, Price{333}).value());

        auto order_ptr2{std::make_shared<Order>(333,
                                                Price{333},
                                                Quantity{444})};

        bh.AddOrder(instr, order_ptr2);
        assert(book_ptr->GetQuantity(Price{333}).value() == bh.GetQuantity(0, Price{333}).value());
        assert(Quantity{888} == bh.GetQuantity(0, Price{333}).value());
    }

    {
        std::string instr = "zzz";
        BookPtr book_ptr = std::make_shared<OrderBook>(instr);
        bh.AddBook(book_ptr);

        auto order_ptr{std::make_shared<Order>(111,
                                               Price{222},
                                               Quantity{333})};

        bh.AddOrder(instr, order_ptr);
        assert(book_ptr->GetQuantity(Price{222}).value() == bh.GetQuantity(1, Price{222}).value());
    }
}

void modify_order_test()
{
    std::string instr = "bbb";
    BookHolder bh;

    BookPtr book_ptr = std::make_shared<OrderBook>(instr);

    bh.AddBook(book_ptr);

    auto order_ptr{std::make_shared<Order>(111,
                                           Price{222},
                                           Quantity{333})};

    bh.AddOrder(instr, order_ptr);

    assert(book_ptr->GetQuantity(Price{222}).value() == Quantity{333});
    assert(bh.GetQuantity(0, Price{222}).value() == Quantity{333});

    auto new_price = Price{444};
    book_ptr->Modify(OrderModify(111, new_price));

    assert(book_ptr->GetQuantity(new_price).value() == Quantity{333});
    assert(bh.GetQuantity(0, new_price).value() == Quantity{333});

    auto new_quantity = Quantity{777};
    book_ptr->Modify(OrderModify(111, new_quantity));

    assert(book_ptr->GetQuantity(new_price).value() == new_quantity);
    assert(bh.GetQuantity(0, new_price).value() == new_quantity);
}

void remove_order_test()
{
}

void order_gen_tests()
{
    BookHolder bh(BookSetFactory({"aaa", "bbb", "ccc"}));

    auto orders_gen{OrdersGenerator(bh.GetInstruments())};
}

TEST_LIST = {

    {"BookHolder int tests", book_holder_init_tests},
    {"OrderBook tests", order_book_tests},
    {"BookHolder add order tests", book_holder_add_order_tests},
    {"Process modify orders tests", modify_order_test},
    {"Process remove orders tests", remove_order_test},

    // {"OrderGen tests", order_gen_tests},

    {NULL, NULL}};
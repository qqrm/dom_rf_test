#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <thread>

#include "OrdersGenerator.hpp"
#include "OrderBook.hpp"
#include "BookHolder.hpp"

using namespace book_holder;

void gen_orders(std::shared_ptr<BookHolder> book_holder)
{
    auto instruments = book_holder->GetInstruments();
    auto orders_gen = OrdersGenerator(instruments);

    for (size_t i = 0; i < 100000; i++)
    {
        auto action = orders_gen.GenerateAction();
    }
}

int main(int argc, char const *argv[])
{
    std::vector<std::string> const INSTRUMENTS{"aaa", "bbb", "ccc"};


    BookSet books = BookSetFactory(INSTRUMENTS);

    auto book_holder = std::make_shared<BookHolder>(books);

    std::jthread gen_thread(gen_orders, book_holder);

    // std::unique_ptr<OrderAction> qwe;

    return 0;
}
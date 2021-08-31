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

        auto res{std::set<std::string>{"aaa", "bbb", "ccc"}};
        auto instr{book_holder.GetInstruments()};
        assert(res == instr);
    }

    {
        BookHolder book_holder;

        assert(0 == book_holder.GetCount());


        auto book = std::make_shared<BookPtr>("bbb");

        book_holder.AddBook(book);

        assert(res == instr);


        assert(res == instr);
    }
}

TEST_LIST = {

    {"BookHolder int tests", book_holder_init_tests},

    {NULL, NULL}};
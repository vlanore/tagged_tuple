#include "../src/fancy_syntax.hpp"
#include "../src/tagged_tuple.hpp"

TOKEN(my_int)
TOKEN(my_string)

int main() {
    std::string is = "hello";
    auto my_tuple = make_tagged_tuple(my_int_ = 2,  //
                                      my_string_ = is);

    auto i = get<my_int>(my_tuple);
    auto s = get<my_string>(my_tuple);

    // other syntax
    auto i2 = my_int_(my_tuple);
    auto s2 = my_string_(my_tuple);
}
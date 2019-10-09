#include <iostream>
#include "../src/fancy_syntax.hpp"
#include "../src/tagged_tuple.hpp"

TOKEN(my_func)

auto make_my_object() {
    int a = 2;
    auto f = [a](auto x){ return x + a; };

    return make_tagged_tuple(my_func_ = f);
}

int main() {
    auto o = make_my_object();
    auto f = get<my_func>(o);
    std::cout << f(1) << "\n"; //3
}
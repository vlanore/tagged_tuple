# tagged_tuple documentation

## Quick intro

This library provides a "tagged tuple" abstraction that is like a `std::tuple` but that can be accessed through names tags instead of integers.

To give a bit of context, here is how a std::tuple works:
```cpp
#include <tuple>

int main() {
    std::tuple<int, std::string, double> my_tuple{1, "a", 3.2};
    int first = std::get<0>(my_tuple);
    std::string second = std::get<1>(my_tuple);

    // better syntax for declaration (auto-deduce template params)
    auto my_tuple2 = std::make_tuple(1, "a", 3.2);
    auto first2 = std::get<0>(my_tuple2);
}
```

Instead, tagged tuples work with tags as indices:

```cpp
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
```

Advantages:
* cannot confuse fields with identical types;
* resistant to reordering!

Drawbacks:
* have to declare tags beforehand;
* slighlty convoluted `make_tagged_tuple` syntax.

## Using tagged tuples instead of objects

Tagged tuples can be used to construct data structure whose field types cannot be written explicitly. This can be seen as a way to build classes with `auto` members.

Here is an example of using a tagged tuple to have a field whose type cannot be written (a lambda):

```cpp
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

```
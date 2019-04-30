/*Copyright or © or Copr. CNRS (2019). Contributors:
- Vincent Lanore. vincent.lanore@gmail.com

This software is a computer program whose purpose is to provide header-only library to create tuple
indexed by type tags.

This software is governed by the CeCILL-C license under French law and abiding by the rules of
distribution of free software. You can use, modify and/ or redistribute the software under the terms
of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy, modify and redistribute
granted by the license, users are provided only with a limited warranty and the software's author,
the holder of the economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated with loading, using,
modifying and/or developing or reproducing the software by the user in light of its specific status
of free software, that may mean that it is complicated to manipulate, and that also therefore means
that it is reserved for developers and experienced professionals having in-depth computer knowledge.
Users are therefore encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or data to be ensured and,
more generally, to use and operate it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <string>
#include "tagged_tuple.hpp"
using std::string;

struct prop1 {};
struct prop2 {};
struct prop3 {};

TEST_CASE("Type map") {
    using namespace type_map;
    using my_map = Map<Pair<prop1, int>, Pair<prop2, double>>;
    using prop1_t = typename my_map::get<prop1>;
    using prop2_t = typename my_map::get<prop2>;
    constexpr int i1 = my_map::get_index<prop1>();
    constexpr int i2 = my_map::get_index<prop2>();

    CHECK((std::is_same<prop1_t, int>::value));
    CHECK((std::is_same<prop2_t, double>::value));
    CHECK(i1 == 0);
    CHECK(i2 == 1);

    using tuple_t = my_map::value_tuple_t;
    CHECK((std::is_same<tuple<int, double>, tuple_t>::value));

    using my_map2 = my_map::add<prop3, string>;
    using prop3_t = typename my_map2::get<prop3>;
    CHECK((std::is_same<prop3_t, string>::value));
    CHECK(my_map2::get_index<prop3>() == 0);  // added in front
    CHECK(my_map2::get_index<prop1>() == 1);
    CHECK(my_map2::get_index<prop2>() == 2);
}

struct alpha {};
struct beta {};
struct gamma_ {};

TEST_CASE("Direct usage of tagged_tuple_t") {
    using namespace type_map;
    using my_map = Map<Pair<alpha, int>, Pair<beta, string>>;
    using my_tuple_t = tagged_tuple_t<my_map>;

    my_tuple_t my_tuple{2, "hello"};
    CHECK(my_tuple.get<alpha>() == 2);
    CHECK(my_tuple.get<beta>() == "hello");

    const my_tuple_t my_const_tuple{3, "hi"};
    CHECK(my_const_tuple.get<alpha>() == 3);
    CHECK(my_const_tuple.get<beta>() == "hi");
}

TEST_CASE("tagged_tuple typedef") {
    using my_tuple_t = tagged_tuple<field<alpha, int>, field<beta, string>>;

    my_tuple_t my_tuple{2, "hello"};  // FIXME order reversed!
    CHECK(my_tuple.get<alpha>() == 2);
    CHECK(my_tuple.get<beta>() == "hello");
}

TEST_CASE("expand tagged tuple") {
    tagged_tuple<field<alpha, int>> t1(17);
    auto t2 = t1.expand<beta>(2.3);
    CHECK(t2.get<alpha>() == 17);
    CHECK(t2.get<beta>() == 2.3);
    auto t3 = t2.expand<struct gamma, std::string>("hello");
    CHECK(t3.get<alpha>() == 17);
    CHECK(t3.get<beta>() == 2.3);
    CHECK(t3.get<gamma>() == "hello");
}

TEST_CASE("Test with unique_ptrs") {
    tagged_tuple<field<beta, int>, field<alpha, std::unique_ptr<double>>> t1(
        3, std::make_unique<double>(2.3));
    CHECK(*t1.get<alpha>() == 2.3);
    CHECK(t1.get<beta>() == 3);

    auto t2 = t1.expand<struct gamma, std::unique_ptr<std::string>>(
        std::make_unique<std::string>("hello"));
    CHECK(t1.get<alpha>() == nullptr);
    CHECK(*t2.get<alpha>() == 2.3);
    CHECK(t2.get<beta>() == 3);
    CHECK(*t2.get<gamma>() == "hello");
}

TEST_CASE("make_tagged_tuple") {
    auto t1 =
        make_tagged_tuple(field_from<alpha>(3), field_from<beta>(std::make_unique<double>(3.2)));
    CHECK(t1.get<alpha>() == 3);
    CHECK(*t1.get<beta>() == 3.2);

    auto t2 = make_tagged_tuple(field_from<alpha>(7), unique_ptr_field<beta>(7.2));
    CHECK(t2.get<alpha>() == 7);
    CHECK(*t2.get<beta>() == 7.2);
}

TEST_CASE("int get") {
    tagged_tuple<field<alpha, int>, field<beta, double>> t1{2, 3.2};
    CHECK(t1.get<0>() == 2);
    CHECK(t1.get<1>() == 3.2);
}
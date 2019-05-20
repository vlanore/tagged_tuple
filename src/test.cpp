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
// #include "debug_tools.hpp"
#include "tagged_tuple.hpp"
using std::string;

struct alpha_ {};
struct beta_ {};
struct gamma_ {};
struct delta_ {};

TEST_CASE("Direct usage of tagged_tuple_t") {
    using namespace minimpl;
    using my_map = map<pair<alpha_, int>, pair<beta_, string>>;
    using my_tuple_t = tagged_tuple_t<my_map>;

    my_tuple_t my_tuple;
    my_tuple.data = std::make_tuple(2, "hello");
    CHECK(get<alpha_>(my_tuple) == 2);
    CHECK(get<beta_>(my_tuple) == "hello");

    const auto& my_const_tuple = my_tuple;
    CHECK(get<alpha_>(my_const_tuple) == 2);
    CHECK(get<beta_>(my_const_tuple) == "hello");
}

// TEST_CASE("tagged_tuple typedef") {
//     using my_tuple_t = tagged_tuple<field<alpha, int>, field<beta, string>>;

//     my_tuple_t my_tuple;
//     my_tuple.data = std::make_tuple(2, "hello");  // FIXME order reversed!
//     CHECK(get<alpha>(my_tuple) == 2);
//     CHECK(get<beta>(my_tuple) == "hello");
// }

// TEST_CASE("push_front tagged tuple") {
//     tagged_tuple<field<alpha, int>> t1;
//     t1.data = std::make_tuple(17);
//     auto t2 = push_front<beta>(t1, 2.3);
//     CHECK(get<alpha>(t2) == 17);
//     CHECK(get<beta>(t2) == 2.3);
//     auto t3 = push_front<struct gamma, std::string>(t2, "hello");
//     CHECK(get<alpha>(t3) == 17);
//     CHECK(get<beta>(t3) == 2.3);
//     CHECK(get<gamma>(t3) == "hello");
// }

// TEST_CASE("Test with unique_ptrs") {
//     tagged_tuple<field<beta, int>, field<alpha, std::unique_ptr<double>>> t1;
//     t1.data = std::make_tuple(3, std::make_unique<double>(2.3));
//     CHECK(get<alpha>(t1) == 2.3);
//     CHECK(get<beta>(t1) == 3);

//     auto t2 = push_front<struct gamma, std::unique_ptr<std::string>>(
//         t1, std::make_unique<std::string>("hello"));
//     // CHECK(get<alpha>(t1) == nullptr); // segfaults (as expected)
//     CHECK(get<alpha>(t2) == 2.3);
//     CHECK(get<beta>(t2) == 3);
//     CHECK(get<gamma>(t2) == "hello");
// }

// TEST_CASE("make_tagged_tuple") {
//     auto t1 =
//         make_tagged_tuple(value_field<alpha>(3),
//         value_field<beta>(std::make_unique<double>(3.2)));
//     CHECK(get<alpha>(t1) == 3);
//     CHECK(get<beta>(t1) == 3.2);

//     auto t2 = make_tagged_tuple(value_field<alpha>(7), unique_ptr_field<beta>(7.2));
//     CHECK(get<alpha>(t2) == 7);
//     CHECK(get<beta>(t2) == 7.2);
// }

// TEST_CASE("make_tagged_tuple ref/nonref/move") {
//     auto c = std::make_unique<double>(3);
//     int a{17}, b{19};
//     auto t = make_tagged_tuple(value_field<alpha>(a), ref_field<beta>(b),
//                                unique_ptr_field<struct gamma>(21), move_field<delta>(c));
//     a = 7;
//     b = 9;
//     CHECK(get<alpha>(t) == 17);
//     CHECK(get<beta>(t) == 9);
//     CHECK(get<gamma>(t) == 21);
//     CHECK(get<delta>(t) == 3);
// }

// TEST_CASE("type_of") {
//     using tuple_t = tagged_tuple<field<alpha, int>, field<beta, double>>;
//     using alpha_t = field_type<tuple_t, alpha>;
//     using beta_t = field_type<tuple_t, beta>;
//     CHECK((std::is_same<alpha_t, int>::value));
//     CHECK((std::is_same<beta_t, double>::value));
// }

// TEST_CASE("recursive tagged tuple") {
//     auto inner = make_tagged_tuple(value_field<alpha>(2));
//     auto outer = make_tagged_tuple(value_field<beta>(inner));
//     CHECK((get<beta, alpha>(outer) == 2));
// }

// TEST_CASE("is_tagged_tuple") {
//     using tuple_t = tagged_tuple<field<alpha, int>, field<beta, double>>;
//     CHECK(is_tagged_tuple<tuple_t>);
//     CHECK(!is_tagged_tuple<double>);
// }

// TEST_CASE("basic type printing") { CHECK(type_to_string<alpha>() == "alpha"); }

// TEST_CASE("struct printing") {
//     using tuple_t = tagged_tuple<field<alpha, int>, field<beta, double>>;
//     tuple_t t;
//     t.data = std::make_tuple(1, 3.2);
//     std::string debug = type_to_string(t);
//     CHECK(debug == "tagged_tuple { int alpha; double beta; }");
// }

// TEST_CASE("Struct printing with non-default constructible stuff") {
//     double a = 3.2;
//     auto inner = make_tagged_tuple(ref_field<alpha>(a));
//     auto outer = make_tagged_tuple(value_field<beta>(inner));
//     CHECK(type_to_string(outer) == "tagged_tuple { tagged_tuple { double& alpha; } beta; }");
// }

// TEST_CASE("Struct printing with unique_pointers") {
//     auto t = make_tagged_tuple(unique_ptr_field<alpha>(3.2));
//     CHECK(type_to_string(t) == "tagged_tuple { unique_ptr<double> alpha; }");
// }

// TEST_CASE("recursive struct printing") {
//     using tuple_t = tagged_tuple<field<alpha, int>, field<beta, double>>;
//     using tuple2_t = tagged_tuple<field<alpha, tuple_t>, field<beta, tuple_t>>;
//     tuple2_t t;
//     get<alpha>(t).data = std::make_tuple(2, 3.9);
//     get<beta>(t).data = std::make_tuple(0, 3.2);
//     std::string debug = type_to_string(t);
//     CHECK(debug ==
//           "tagged_tuple { tagged_tuple { int alpha; double beta; } alpha; tagged_tuple { int "
//           "alpha; double beta; } beta; }");
// }

// struct Tag1 {};
// struct Tag2 {};
// struct Tag3 {};

// TEST_CASE("Context check tag presence") {
//     using namespace utils;
//     auto t = tagged_tuple_t<type_map::Map<Pair<alpha, int>>, Context<Tag1, Tag3>>();
//     CHECK((has_tag<decltype(t), Tag1>::value));
//     CHECK((!has_tag<decltype(t), Tag2>::value));
//     CHECK((has_tag<decltype(t), Tag3>::value));
// }

// TEST_CASE("Get property") {
//     using namespace utils;
//     using namespace type_map;
//     auto t = tagged_tuple_t<Map<Pair<alpha, int>>, Context<>,
//                             Map<Pair<alpha, Tag1>, Pair<beta, Tag3>>>();
//     CHECK((std::is_same<get_property<decltype(t), alpha>, Tag1>::value));
//     CHECK((std::is_same<get_property<decltype(t), beta>, Tag3>::value));
// }

// TEST_CASE("build ttuples with tags") {
//     auto t = make_tagged_tuple(value_field<alpha>(3.0), unique_ptr_field<beta, int>(2));
//     CHECK((!has_tag<decltype(t), Tag1>::value));
//     CHECK((!has_tag<decltype(t), Tag2>::value));

//     auto t2 = add_tag<Tag2>(t);
//     CHECK(get<beta>(t2) == 2);
//     CHECK((!has_tag<decltype(t2), Tag1>::value));
//     CHECK((has_tag<decltype(t2), Tag2>::value));

//     auto t3 = make_tagged_tuple(value_field<alpha>(3.0), tag<Tag3>(),
//                                 unique_ptr_field<beta, int>(2), tag<Tag2>());
//     CHECK((!has_tag<decltype(t3), Tag1>::value));
//     CHECK((has_tag<decltype(t3), Tag2>::value));
//     CHECK((has_tag<decltype(t3), Tag3>::value));
// }

// TEST_CASE("build ttuples with props") {
//     auto t = make_tagged_tuple(value_field<alpha>(3.0), unique_ptr_field<beta, int>(2));

//     auto t2 = add_prop<alpha, Tag2>(t);
//     CHECK(get<beta>(t2) == 2);
//     CHECK((std::is_same<get_property<decltype(t2), alpha>, Tag2>::value));

//     auto t3 =
//         make_tagged_tuple(value_field<alpha>(3.0), property<beta, Tag2>(), tag<Tag3>(),
//                           unique_ptr_field<beta, int>(2), property<alpha, Tag3>(), tag<Tag2>());
//     CHECK((std::is_same<get_property<decltype(t3), beta>, Tag2>::value));
//     CHECK((std::is_same<get_property<decltype(t3), alpha>, Tag3>::value));
//     CHECK((has_tag<decltype(t3), Tag2>::value));
//     CHECK((has_tag<decltype(t3), Tag3>::value));
// }
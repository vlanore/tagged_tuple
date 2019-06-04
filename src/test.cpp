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

TEST_CASE("Constructors and get (basic usage)") {
    using my_tuple_t = tagged_tuple<no_metadata, field<alpha_, int>, field<beta_, double>,
                                    field<gamma_, std::string>>;
    struct my_struct {
        int a;
        double b;
        std::string c;
    };

    CHECK(sizeof(my_tuple_t) == sizeof(my_struct));
    CHECK(is_tagged_tuple<my_tuple_t>::value);
    CHECK(not is_tagged_tuple<my_struct>::value);

    my_tuple_t my_tuple;
    my_tuple.data = std::make_tuple(2, 3.0, "hello");
    CHECK(get<alpha_>(my_tuple) == 2);
    CHECK(get<gamma_>(my_tuple) == "hello");

    const auto& my_const_tuple = my_tuple;
    CHECK(get<alpha_>(my_const_tuple) == 2);
    CHECK(get<gamma_>(my_const_tuple) == "hello");

    my_tuple_t my_tuple2{tuple_construct(), 4, 3.2, "hi"};
    CHECK(get<alpha_>(my_tuple2) == 4);
    CHECK(get<gamma_>(my_tuple2) == "hi");
}

TEST_CASE("push_front tagged tuple") {
    tagged_tuple<no_metadata, field<alpha_, int>> t1;
    t1.data = std::make_tuple(17);
    auto t2 = push_front<beta_>(2.3, t1);
    CHECK(get<alpha_>(t2) == 17);
    CHECK(get<beta_>(t2) == 2.3);
    auto t3 = push_front<gamma_, std::string>("hello", t2);
    CHECK(get<alpha_>(t3) == 17);
    CHECK(get<beta_>(t3) == 2.3);
    CHECK(get<gamma_>(t3) == "hello");
}

TEST_CASE("Test with unique_ptrs") {
    tagged_tuple<no_metadata, field<beta_, int>, field<alpha_, std::unique_ptr<double>>> t1;
    t1.data = std::make_tuple(3, std::make_unique<double>(2.3));
    CHECK(get<alpha_>(t1) == 2.3);
    CHECK(get<beta_>(t1) == 3);

    auto t2 = push_front<gamma_, std::unique_ptr<std::string>>(
        std::make_unique<std::string>("hello"), t1);
    // CHECK(get<alpha_>(t1) == nullptr); // segfaults (as expected)
    CHECK(get<alpha_>(t2) == 2.3);
    CHECK(get<beta_>(t2) == 3);
    CHECK(get<gamma_>(t2) == "hello");
}

TEST_CASE("make_tagged_tuple") {
    auto t1 = make_tagged_tuple(value_field<alpha_>(3),
                                value_field<beta_>(std::make_unique<double>(3.2)));
    CHECK(get<alpha_>(t1) == 3);
    CHECK(get<beta_>(t1) == 3.2);

    auto t2 = make_tagged_tuple(value_field<alpha_>(7), unique_ptr_field<beta_>(7.2));
    CHECK(get<alpha_>(t2) == 7);
    CHECK(get<beta_>(t2) == 7.2);
}

TEST_CASE("make_tagged_tuple ref/nonref/move") {
    auto c = std::make_unique<double>(3);
    int a{17}, b{19};
    auto t = make_tagged_tuple(value_field<alpha_>(a), ref_field<beta_>(b),
                               unique_ptr_field<gamma_>(21), move_field<delta_>(c));
    a = 7;
    b = 9;
    CHECK(get<alpha_>(t) == 17);
    CHECK(get<beta_>(t) == 9);
    CHECK(get<gamma_>(t) == 21);
    CHECK(get<delta_>(t) == 3);
}

TEST_CASE("field type") {
    using tuple_t = tagged_tuple<no_metadata, field<alpha_, int>, field<beta_, double>>;
    using alpha_t = field_type<alpha_, tuple_t>;
    using beta_t = field_type<beta_, tuple_t>;
    CHECK((std::is_same<alpha_t, int>::value));
    CHECK((std::is_same<beta_t, double>::value));
}

TEST_CASE("recursive tagged tuple") {
    auto inner = make_tagged_tuple(value_field<alpha_>(2));
    auto outer = make_tagged_tuple(value_field<beta_>(inner));
    CHECK((get<beta_, alpha_>(outer) == 2));
}

TEST_CASE("Metadata") {
    using m = metadata<type_list<alpha_, beta_>,
                       type_map<property<alpha_, double>, property<gamma_, char>>>;
    using m2 = metadata<alpha_, type_list<beta_, gamma_>>;
    CHECK(is_metadata<m>::value);
    CHECK(not is_metadata<m2>::value);
    CHECK(metadata_has_tag<alpha_, m>::value);
    CHECK(metadata_has_tag<beta_, m>::value);
    CHECK(not metadata_has_tag<gamma_, m>::value);
    CHECK(metadata_has_property<alpha_, m>::value);
    CHECK(not metadata_has_property<beta_, m>::value);
    CHECK(metadata_has_property<gamma_, m>::value);
    CHECK(std::is_same<metadata_get_property<alpha_, m>, double>::value);
    CHECK(std::is_same<metadata_get_property<gamma_, m>, char>::value);
}

TEST_CASE("Tuple metadata access") {
    using m = metadata<type_list<alpha_, beta_>,
                       type_map<property<alpha_, double>, property<gamma_, char>>>;
    auto t = make_tagged_tuple<m>(value_field<alpha_>(2), value_field<beta_>(3.2));
    CHECK(has_tag<alpha_>(t));
    CHECK(has_tag<beta_>(t));
    CHECK(not has_tag<gamma_>(t));
}

// TEST_CASE("basic type printing") { CHECK(type_to_string<alpha_>() == "alpha_"); }

// TEST_CASE("struct printing") {
//     using tuple_t = tagged_tuple<field<alpha_, int>, field<beta_, double>>;
//     tuple_t t;
//     t.data = std::make_tuple(1, 3.2);
//     std::string debug = type_to_string(t);
//     CHECK(debug == "tagged_tuple { int alpha_; double beta_; }");
// }

// TEST_CASE("Struct printing with non-default constructible stuff") {
//     double a = 3.2;
//     auto inner = make_tagged_tuple(ref_field<alpha_>(a));
//     auto outer = make_tagged_tuple(value_field<beta_>(inner));
//     CHECK(type_to_string(outer) == "tagged_tuple { tagged_tuple { double& alpha_; } beta_;
// }");
// }

// TEST_CASE("Struct printing with unique_pointers") {
//     auto t = make_tagged_tuple(unique_ptr_field<alpha_>(3.2));
//     CHECK(type_to_string(t) == "tagged_tuple { unique_ptr<double> alpha_; }");
// }

// TEST_CASE("recursive struct printing") {
//     using tuple_t = tagged_tuple<field<alpha_, int>, field<beta_, double>>;
//     using tuple2_t = tagged_tuple<field<alpha_, tuple_t>, field<beta_, tuple_t>>;
//     tuple2_t t;
//     get<alpha_>(t).data = std::make_tuple(2, 3.9);
//     get<beta_>(t).data = std::make_tuple(0, 3.2);
//     std::string debug = type_to_string(t);
//     CHECK(debug ==
//           "tagged_tuple { tagged_tuple { int alpha_; double beta_; } alpha_; tagged_tuple {
// int "
//           "alpha_; double beta_; } beta_; }");
// }
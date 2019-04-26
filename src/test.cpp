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

#include "tagged_tuple.hpp"

struct prop1 {};
struct prop2 {};

TEST_CASE("Type map") {
    using my_map = TypeMap<TypePair<prop1, int>, TypePair<prop2, double>>;
    using prop1_t = typename my_map::get_t<prop1>;
    using prop2_t = typename my_map::get_t<prop2>;

    CHECK((std::is_same<prop1_t, int>::value));
    CHECK((std::is_same<prop2_t, double>::value));
}

struct alpha {};
struct beta {};
struct gamma {};

TEST_CASE("Basic tuple test") {
    using my_fields = std::tuple<field<alpha, int>, field<beta, std::string>>;
    SUBCASE("Manual tests of under-the-hood things") {
        using namespace helper;
        using my_tags = decltype(get_tags(my_fields()));
        using my_tuple_t = decltype(get_tuple(my_fields()));

        my_tuple_t my_tuple{2, "hello"};

        using tlist = std::tuple<alpha, beta>;
        bool ok_tags = std::is_same<my_tags, tlist>::value;
        CHECK(ok_tags);
        CHECK(get_index<alpha>(tlist()) == 0);
        CHECK(get_index<beta>(tlist()) == 1);
        // CHECK(get_index<gamma>(tlist()) == -1); // triggers static assert

        auto a = get<get_index<beta>(tlist())>(my_tuple);
        CHECK(a == "hello");
        auto b = get<get_index<alpha>(tlist())>(my_tuple);
        CHECK(b == 2);
    }
    SUBCASE("Using user-level interface") {
        using hello_t = ttuple<my_fields>;
        hello_t my_other_struct{3, "hi"};
        CHECK(get<alpha>(my_other_struct) == 3);
        CHECK(get<beta>(my_other_struct) == "hi");
    }
}

TEST_CASE("Multiple levels") {
    using sttuple_t = ttuple<std::tuple<field<alpha, int>>>;
    using cttuple_t = ttuple<std::tuple<field<beta, sttuple_t>>>;
    cttuple_t my_tuple(7);
    CHECK(get<beta, alpha>(my_tuple) == 7);
}

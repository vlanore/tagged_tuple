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

#include "type_map.hpp"
using std::string;

template <class TagMap>
// tagged tuple class (just a tuple wrapper with added tags and static funcs)
struct tagged_tuple_t {
    using tuple_t = typename TagMap::value_tuple_t;
    tuple_t data;

    template <class... Args>
    // constructor that just perfect-forwards arguments to tuple constructor
    tagged_tuple_t(Args&&... args) : data(std::forward<Args>(args)...) {}

    template <class Tag>
    // get a field of the tagged tuple by tag (returns a reference)
    auto& get() {
        constexpr int index = TagMap::template get_index<Tag>();
        return std::get<index>(data);
    }

    template <class Tag>
    // get a field of the tagged tuple by tag (returns a constant reference)
    const auto& get() const {
        constexpr int index = TagMap::template get_index<Tag>();
        return std::get<index>(data);
    }

    template <class Tag, class Type, size_t... Is>
    auto expand_helper(Type&& new_data, std::index_sequence<Is...>) {
        using new_tagmap = typename TagMap::template add<Tag, Type>;
        // important: this moves old data (e.g., in case of unique_ptr  )
        return tagged_tuple_t<new_tagmap>(std::forward<Type>(new_data),
                                          std::move(std::get<Is>(data))...);
    }

    template <class Tag, class Type>
    // adds a field to the struct and returns a new struct
    // WARNING: might invalidate old struct by moveing its contents to new struct!
    auto expand(Type&& new_data) {
        auto is = std::make_index_sequence<std::tuple_size<tuple_t>::value>();
        return expand_helper<Tag>(std::forward<Type>(new_data), is);
    }
};

template <class Tag, class Type>
// to be used in field list declaration
struct field {};

namespace helper {
    auto map_from_fields(tuple<>) { return type_map::Map<>(); }

    template <class Tag, class Type, class... Rest>
    auto map_from_fields(tuple<field<Tag, Type>, Rest...>) {
        using recursive_call = decltype(map_from_fields(tuple<Rest...>()));
        using add_field = typename recursive_call::template add<Tag, Type>;
        return add_field();
    }
};  // namespace helper

template <class... Fields>
// alias used to construct ttuple type from a list of fields (a list of "field" objects)
using tagged_tuple = tagged_tuple_t<decltype(helper::map_from_fields(tuple<Fields...>()))>;

template <class Tag, class Type>
// to be used in make_tagged_tuple
struct FieldFrom {
    Type data;
    FieldFrom(Type&& data) : data(std::forward<Type>(data)) {}
    using tag = Tag;
    using type = Type;
};

template <class Tag, class Type>
auto field_from(Type&& data) {
    return FieldFrom<Tag, Type>(std::forward<Type>(data));
}

namespace helper {

    auto make_tagged_tuple_helper() { return tagged_tuple<>(); }

    template <class Tag, class Type, class... Rest>
    auto make_tagged_tuple_helper(FieldFrom<Tag, Type> f1, Rest&&... rest) {
        auto recursive_call = make_tagged_tuple_helper(std::forward<Rest>(rest)...);
        return recursive_call.template expand<Tag>(std::move(f1.data));
    }

};  // namespace helper

template <class... Fields>
auto make_tagged_tuple(Fields&&... fields) {
    return helper::make_tagged_tuple_helper(std::forward<Fields>(fields)...);
}

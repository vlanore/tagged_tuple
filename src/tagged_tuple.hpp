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

#pragma once

#include <memory>
#include "type_map.hpp"
using std::string;

//==================================================================================================
// Tagged tuple class

struct TaggedTupleTag {};
struct ForwardToTupleConstructor {};

template <class TagMap>
// tagged tuple class (just a tuple wrapper with added tags and static funcs)
struct tagged_tuple_t : TaggedTupleTag {
    using tuple_t = typename TagMap::value_tuple_t;
    using tag_map = TagMap;

    tuple_t data;

    tagged_tuple_t() = default;
    tagged_tuple_t(const tagged_tuple_t&) = default;
    tagged_tuple_t(tagged_tuple_t&&) = default;

    template <class... TupleConstructorArgs>
    explicit tagged_tuple_t(ForwardToTupleConstructor, TupleConstructorArgs&&... args)
        : data(std::forward<TupleConstructorArgs>(args)...) {}

    // get a field of the tagged tuple by tag (returns a reference)
    template <class Tag>
    auto& get();

    // get a field of the tagged tuple by tag (returns a constant reference)
    template <class Tag>
    const auto& get() const;

    // recursive version of getter (for tagged tuple parameters)
    template <class First, class Second, class... Rest>
    auto& get();

    // recursive version of getter (for tagged tuple parameters)
    template <class First, class Second, class... Rest>
    const auto& get() const;

    // type of field
    template <class Tag>
    using type_of = std::tuple_element_t<tag_map::template get_index<Tag>(), tuple_t>;
};

//==================================================================================================
// get element from tag

// get a field of the tagged tuple by tag (returns a reference)
template <class TagMap>
template <class Tag>
auto& tagged_tuple_t<TagMap>::get() {
    constexpr int index = TagMap::template get_index<Tag>();
    return std::get<index>(data);
}

// get a field of the tagged tuple by tag (returns a constant reference)
template <class TagMap>
template <class Tag>
const auto& tagged_tuple_t<TagMap>::get() const {
    constexpr int index = TagMap::template get_index<Tag>();
    return std::get<index>(data);
}

// recursive version of getter (for tagged tuple parameters)
template <class TagMap>
template <class First, class Second, class... Rest>
auto& tagged_tuple_t<TagMap>::get() {
    return get<First>().template get<Second, Rest...>();
}

// recursive version of getter (for tagged tuple parameters)
template <class TagMap>
template <class First, class Second, class... Rest>
const auto& tagged_tuple_t<TagMap>::get() const {
    return get<First>().template get<Second, Rest...>();
}

//==================================================================================================
// push_front

namespace helper {
    template <class Tag, class TTuple, class Type, size_t... Is>
    auto push_front_helper(TTuple& t, Type&& new_data, std::index_sequence<Is...>) {
        using old_tagmap = typename TTuple::tag_map;
        using new_tagmap = typename old_tagmap::template push_front<Tag, Type>;
        // important: this moves old data (e.g., in case of unique_ptr  )
        return tagged_tuple_t<new_tagmap>(ForwardToTupleConstructor(), std::forward<Type>(new_data),
                                          std::move(std::get<Is>(t.data))...);
    }
};  // namespace helper

// adds a field to the struct and returns a new struct
// WARNING: might invalidate old struct by moving its contents to new struct
template <class Tag, class Type, class TTuple>
auto push_front(TTuple& t, Type&& new_data) {
    using underlying_tuple_t = typename TTuple::tuple_t;

    // build index sequence to be able to unpack tuple into tagged_tuple_t constructor
    // unpacking happens in helper
    auto is = std::make_index_sequence<std::tuple_size<underlying_tuple_t>::value>();
    return helper::push_front_helper<Tag>(t, std::forward<Type>(new_data), is);
}

//==================================================================================================
// tagged_tuple type creation

// to be used in field list declaration
template <class Tag, class Type>
struct field {};

namespace helper {
    auto map_from_fields(tuple<>) { return type_map::Map<>(); }

    template <class Tag, class Type, class... Rest>
    auto map_from_fields(tuple<field<Tag, Type>, Rest...>) {
        using recursive_call = decltype(map_from_fields(tuple<Rest...>()));
        using add_field = typename recursive_call::template push_front<Tag, Type>;
        return add_field();
    }
};  // namespace helper

// alias used to construct ttuple type from a list of fields (a list of "field" objects)
template <class... Fields>
using tagged_tuple = tagged_tuple_t<decltype(helper::map_from_fields(tuple<Fields...>()))>;

//==================================================================================================
// make tagged tuple from values

// to be used in make_tagged_tuple
template <class Tag, class Type>
struct TagValuePair {
    Type data;
    template <class InitType>  // might have different ref-ness than type
    explicit TagValuePair(InitType&& data) : data(std::forward<InitType>(data)) {}
};

// to be used in make_tagged_tuple calls
template <class Tag, class Type>
auto value_field(Type&& data) {
    using nonref_type = std::remove_reference_t<Type>;
    return TagValuePair<Tag, nonref_type>{std::forward<Type>(data)};
}

// to be used in make_tagged_tuple calls
template <class Tag, class Type>
auto ref_field(Type& data) {
    return TagValuePair<Tag, Type&>(data);
}

// to be used in make_tagged_tuple calls; builds a unique pointer to Type from Type constructor args
template <class Tag, class Type>
auto unique_ptr_field(Type&& data) {
    return TagValuePair<Tag, std::unique_ptr<Type>>(
        std::make_unique<Type>(std::forward<Type>(data)));
}

namespace helper {
    auto make_tagged_tuple_helper() { return tagged_tuple<>(); }

    template <class Tag, class Type, class... Rest>
    auto make_tagged_tuple_helper(TagValuePair<Tag, Type>&& f1, Rest&&... rest) {
        auto recursive_call = make_tagged_tuple_helper(std::forward<Rest>(rest)...);
        return push_front<Tag>(recursive_call, std::forward<Type>(f1.data));
    }
};  // namespace helper

// builds a tuple from a list of fields with values (auto-deduces field types from values)
template <class... Fields>
auto make_tagged_tuple(Fields&&... fields) {
    return helper::make_tagged_tuple_helper(std::move(fields)...);
}

//==================================================================================================
// is_tagged_tuple type trait

template <class T>
constexpr bool is_tagged_tuple = std::is_base_of<TaggedTupleTag, T>::value;

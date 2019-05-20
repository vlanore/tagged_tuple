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
#include "minimpl/src/map.hpp"

//==================================================================================================
// Tagged tuple class

struct TaggedTuple {};
struct ForwardToTupleConstructor {};

// tagged tuple class (just a tuple wrapper with added tags and static funcs)
template <class Fields = minimpl::map<>, class Tags = minimpl::list<>,
          class Properties = minimpl::map<>>
struct tagged_tuple_t : TaggedTuple {
    static_assert(minimpl::is_map<Fields>::value, "Fields should be a map");
    static_assert(minimpl::is_list<Tags>::value, "Tags should be a list");
    static_assert(minimpl::is_map<Properties>::value, "Properties should be a map");

    using tuple_t = minimpl::map_value_tuple_t<Fields>;
    using fields = Fields;
    using tags = Tags;
    using properties = Properties;

    tuple_t data;

    tagged_tuple_t() = default;
    tagged_tuple_t(const tagged_tuple_t&) = default;
    tagged_tuple_t(tagged_tuple_t&&) = default;

    template <class... TupleConstructorArgs>
    explicit tagged_tuple_t(ForwardToTupleConstructor, TupleConstructorArgs&&... args)
        : data(std::forward<TupleConstructorArgs>(args)...) {}
};

//==================================================================================================
// is_tagged_tuple type traits

template <class T>
using is_tagged_tuple = std::is_base_of<TaggedTuple, T>;

// namespace helper {
//     template <class T>
//     constexpr bool select_ttuple_ptr(utils::Type<std::unique_ptr<tagged_tuple_t<T>>>) {
//         return true;
//     }

//     template <class T>
//     constexpr bool select_ttuple_ptr(utils::Type<tagged_tuple_t<T>>) {
//         return true;
//     }

//     template <class T>
//     constexpr bool select_ttuple_ptr(utils::Type<T>) {
//         return false;
//     }
// };  // namespace helper

// template <class T>
// constexpr bool is_tagged_tuple_or_ptr = helper::select_ttuple_ptr(utils::Type<T>());

//==================================================================================================
// get element from tag

namespace helper {
    template <class Type>
    auto& get_ref(std::unique_ptr<Type>& rhs) {
        return *rhs;
    }

    template <class Type>
    auto& get_ref(Type& rhs) {
        return rhs;
    }

    template <class Type>
    const auto& get_ref(const std::unique_ptr<Type>& rhs) {
        return *rhs;
    }

    template <class Type>
    const auto& get_ref(const Type& rhs) {
        return rhs;
    }
};  // namespace helper

// get a field of the tagged tuple by tag (returns a reference)
template <class Tag, class TTuple, typename = std::enable_if_t<is_tagged_tuple<TTuple>::value>>
auto& get(TTuple& tuple) {
    constexpr int index = minimpl::map_element_index<typename TTuple::fields, Tag>::value;
    return helper::get_ref(std::get<index>(tuple.data));
}

// get a field of the tagged tuple by tag (returns a constant reference)
template <class Tag, class TTuple, typename = std::enable_if_t<is_tagged_tuple<TTuple>::value>>
const auto& get(const TTuple& tuple) {
    constexpr int index = minimpl::map_element_index<typename TTuple::fields, Tag>::value;
    return helper::get_ref(std::get<index>(tuple.data));
}

// // recursive version of getter (for tagged tuple parameters)
// template <class First, class Second, class... Rest, class TagMap>
// auto& get(tagged_tuple_t<TagMap>& tuple) {
//     static_assert(is_tagged_tuple_or_ptr<typename TagMap::template type_of<First>>,
//                   "Field tag passed to recursive get is not a tagged tuple");
//     return get<Second, Rest...>(get<First>(tuple));
// }

// // recursive version of getter (for tagged tuple parameters)
// template <class First, class Second, class... Rest, class TagMap>
// const auto& get(const tagged_tuple_t<TagMap>& tuple) {
//     static_assert(is_tagged_tuple_or_ptr<typename TagMap::template type_of<First>>,
//                   "Field tag passed to recursive get is not a tagged tuple");
//     return get<Second, Rest...>(get<First>(tuple));
// }

// //==================================================================================================
// // push_front

// namespace helper {
//     template <class T>
//     auto& copy_or_move(utils::Type<T&>, T& ref) {
//         return ref;
//     }

//     template <class T>
//     auto&& copy_or_move(utils::Type<T>, T& ref) {
//         return std::move(ref);
//     }

//     template <class Tag, class TTuple, class Type, size_t... Is>
//     auto push_front_helper(TTuple& t, Type&& new_data, std::index_sequence<Is...>) {
//         using old_tagmap = typename TTuple::tag_map;
//         using new_tagmap = typename old_tagmap::template push_front<Tag, Type>;
//         // important: this moves unique pointers
//         return tagged_tuple_t<new_tagmap, typename TTuple::context, typename TTuple::properties>(
//             ForwardToTupleConstructor(), std::forward<Type>(new_data),
//             copy_or_move(utils::Type<std::tuple_element_t<Is, typename TTuple::tuple_t>>(),
//                          std::get<Is>(t.data))...);
//     }
// };  // namespace helper

// // adds a field to the struct and returns a new struct
// // WARNING: invalidates (moves) all unique pointers in old struct
// template <class Tag, class Type, class TTuple>
// auto push_front(TTuple& t, Type&& new_data) {
//     using underlying_tuple_t = typename TTuple::tuple_t;

//     // build index sequence to be able to unpack tuple into tagged_tuple_t constructor
//     // unpacking happens in helper
//     auto is = std::make_index_sequence<std::tuple_size<underlying_tuple_t>::value>();
//     return helper::push_front_helper<Tag>(t, std::forward<Type>(new_data), is);
// }

// //==================================================================================================
// // adding tags

// template <class Tag, class TagMap, class Context, class Properties>
// auto add_tag(tagged_tuple_t<TagMap, Context, Properties>& t) {
//     return tagged_tuple_t<TagMap, decltype(Context::template add_tag<Tag>()), Properties>(
//         ForwardToTupleConstructor(), std::move(t.data));
// }

// //==================================================================================================
// // adding props

// template <class Name, class Value, class TagMap, class Context, class Properties>
// auto add_prop(tagged_tuple_t<TagMap, Context, Properties>& t) {
//     return tagged_tuple_t<TagMap, Context, typename Properties::template push_front<Name,
//     Value>>(
//         ForwardToTupleConstructor(), std::move(t.data));
// }

// //==================================================================================================
// // tagged_tuple type creation

// // to be used in field list declaration
// template <class Tag, class Type>
// struct field {};

// namespace helper {
//     auto map_from_fields(tuple<>) { return type_map::Map<>(); }

//     template <class Tag, class Type, class... Rest>
//     auto map_from_fields(tuple<field<Tag, Type>, Rest...>) {
//         using recursive_call = decltype(map_from_fields(tuple<Rest...>()));
//         using add_field = typename recursive_call::template push_front<Tag, Type>;
//         return add_field();
//     }
// };  // namespace helper

// // alias used to construct ttuple type from a list of fields (a list of "field" objects)
// template <class... Fields>
// using tagged_tuple = tagged_tuple_t<decltype(helper::map_from_fields(tuple<Fields...>()))>;

// //==================================================================================================
// // make tagged tuple from values

// // to be used in make_tagged_tuple
// template <class Tag, class Type>
// struct TagValuePair {
//     Type data;
//     template <class InitType>  // might have different ref-ness than type
//     explicit TagValuePair(InitType&& data) : data(std::forward<InitType>(data)) {}
// };

// // to be used in make_tagged_tuple calls
// template <class Tag, class Type>
// auto value_field(Type&& data) {
//     using nonref_type = std::remove_reference_t<Type>;
//     return TagValuePair<Tag, nonref_type>{std::forward<Type>(data)};
// }

// // to be used in make_tagged_tuple calls
// template <class Tag, class Type>
// auto move_field(Type& data) {
//     return TagValuePair<Tag, Type>{std::move(data)};
// }

// // to be used in make_tagged_tuple calls
// template <class Tag, class Type>
// auto ref_field(Type& data) {
//     return TagValuePair<Tag, Type&>(data);
// }

// // to be used in make_tagged_tuple calls; builds a unique pointer to Type from Type constructor
// args template <class Tag, class Type> auto unique_ptr_field(Type&& data) {
//     return TagValuePair<Tag, std::unique_ptr<Type>>(
//         std::make_unique<Type>(std::forward<Type>(data)));
// }

// template <class Tag>
// using tag = utils::Type<Tag>;

// template <class PropName, class PropValue>
// using property = utils::Pair<PropName, PropValue>;

// namespace helper {
//     auto make_tagged_tuple_helper() { return tagged_tuple<>(); }

//     template <class Tag, class... Rest>
//     auto make_tagged_tuple_helper(tag<Tag>, Rest&&...);  // so it's declared in the other
//     overload

//     template <class Name, class Value, class... Rest>
//     auto make_tagged_tuple_helper(property<Name, Value>, Rest&&...);

//     template <class Tag, class Type, class... Rest>
//     auto make_tagged_tuple_helper(TagValuePair<Tag, Type>&& f1, Rest&&... rest) {
//         auto recursive_call = make_tagged_tuple_helper(std::forward<Rest>(rest)...);
//         return push_front<Tag>(recursive_call, std::forward<Type>(f1.data));
//     }

//     template <class Tag, class... Rest>
//     auto make_tagged_tuple_helper(tag<Tag>, Rest&&... rest) {
//         auto recursive_call = make_tagged_tuple_helper(std::forward<Rest>(rest)...);
//         return add_tag<Tag>(recursive_call);
//     }

//     template <class Name, class Value, class... Rest>
//     auto make_tagged_tuple_helper(property<Name, Value>, Rest&&... rest) {
//         auto recursive_call = make_tagged_tuple_helper(std::forward<Rest>(rest)...);
//         return add_prop<Name, Value>(recursive_call);
//     }
// };  // namespace helper

// // builds a tuple from a list of fields with values (auto-deduces field types from values)
// template <class... Fields>
// auto make_tagged_tuple(Fields&&... fields) {
//     return helper::make_tagged_tuple_helper(std::move(fields)...);
// }

// //==================================================================================================
// // various type aliases to introspect tuple

// template <class TTuple, class Tag>
// using has_tag = typename TTuple::context::template has_tag<Tag>;

// // type of field
// template <class TTuple, class Tag>
// using field_type =
//     std::tuple_element_t<TTuple::tag_map::template get_index<Tag>(), typename TTuple::tuple_t>;

// // get property
// template <class TTuple, class Tag>
// using get_property = typename TTuple::properties::template get<Tag>;
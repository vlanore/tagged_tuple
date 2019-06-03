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

#include "minimpl/src/type_map.hpp"

template <class T, class U>
using field = type_pair<T, U>;

template <class Metadata, class... Fields>
struct tagged_tuple {
    using field_map = type_map<Fields...>;
    using metadata = Metadata;

    map_value_list_t<field_map> data;
};

// namespace helper {
//     using namespace minimpl;
// };
// using std::tuple;

//==================================================================================================
// Tagged tuple class

// struct TaggedTuple {};
// struct ForwardToTupleConstructor {};

// // tagged tuple class (just a tuple wrapper with added tags and static funcs)
// template <class Fields = minimpl::map<>, class Tags = minimpl::list<>,
//           class Properties = minimpl::map<>>
// struct tagged_tuple_t : TaggedTuple {
//     static_assert(minimpl::is_map<Fields>::value, "Fields should be a map");
//     static_assert(minimpl::is_list<Tags>::value, "Tags should be a list");
//     static_assert(minimpl::is_map<Properties>::value, "Properties should be a map");

//     using tuple_t = typename minimpl::map_value_list_t<Fields>::tuple;
//     using fields = Fields;
//     using tags = Tags;
//     using properties = Properties;

//     tuple_t data;

//     tagged_tuple_t() = default;

//     template <class... TupleConstructorArgs>
//     explicit tagged_tuple_t(ForwardToTupleConstructor, TupleConstructorArgs&&... args)
//         : data(std::forward<TupleConstructorArgs>(args)...) {}
// };

// //==================================================================================================
// // is_tagged_tuple type traits

// template <class T>
// using is_tagged_tuple = std::is_base_of<TaggedTuple, T>;

// namespace helper {
//     template <class... T>
//     constexpr bool select_ttuple_ptr(box<std::unique_ptr<tagged_tuple_t<T...>>>) {
//         return true;
//     }

//     template <class... T>
//     constexpr bool select_ttuple_ptr(box<tagged_tuple_t<T...>>) {
//         return true;
//     }

//     template <class T>
//     constexpr bool select_ttuple_ptr(box<T>) {
//         return false;
//     }
// };  // namespace helper

// template <class T>
// constexpr bool is_tagged_tuple_or_ptr = helper::select_ttuple_ptr(minimpl::box<T>());

// //==================================================================================================
// // get element from tag

// namespace helper {
//     template <class Type>
//     auto& get_ref(std::unique_ptr<Type>& rhs) {
//         return *rhs;
//     }

//     template <class Type>
//     auto& get_ref(Type& rhs) {
//         return rhs;
//     }

//     template <class Type>
//     const auto& get_ref(const std::unique_ptr<Type>& rhs) {
//         return *rhs;
//     }

//     template <class Type>
//     const auto& get_ref(const Type& rhs) {
//         return rhs;
//     }
// };  // namespace helper

// // get a field of the tagged tuple by tag (returns a reference)
// template <class Tag, class TTuple, typename = std::enable_if_t<is_tagged_tuple<TTuple>::value>>
// auto& get(TTuple& tuple) {
//     constexpr int index = minimpl::map_element_index<typename TTuple::fields, Tag>::value;
//     return helper::get_ref(std::get<index>(tuple.data));
// }

// // get a field of the tagged tuple by tag (returns a constant reference)
// template <class Tag, class TTuple, typename = std::enable_if_t<is_tagged_tuple<TTuple>::value>>
// const auto& get(const TTuple& tuple) {
//     constexpr int index = minimpl::map_element_index<typename TTuple::fields, Tag>::value;
//     return helper::get_ref(std::get<index>(tuple.data));
// }

// // recursive version of getter (for tagged tuple parameters)
// template <class First, class Second, class... Rest, class Fields, class Tags, class Properties>
// auto& get(tagged_tuple_t<Fields, Tags, Properties>& tuple) {
//     static_assert(is_tagged_tuple_or_ptr<minimpl::map_element_t<Fields, First>>,
//                   "Field tag passed to recursive get is not a tagged tuple");
//     return get<Second, Rest...>(get<First>(tuple));
// }

// // recursive version of getter (for tagged tuple parameters)
// template <class First, class Second, class... Rest, class Fields, class Tags, class Properties>
// const auto& get(const tagged_tuple_t<Fields, Tags, Properties>& tuple) {
//     static_assert(is_tagged_tuple_or_ptr<minimpl::map_element_t<Fields, First>>,
//                   "Field tag passed to recursive get is not a tagged tuple");
//     return get<Second, Rest...>(get<First>(tuple));
// }

// //==================================================================================================
// // push_front

// namespace helper {
//     template <class T>
//     auto& copy_or_move(box<T&>, T& ref) {
//         return ref;
//     }

//     template <class T>
//     auto&& copy_or_move(box<T>, T& ref) {
//         return std::move(ref);
//     }

//     template <class Tag, class TTuple, class Type, size_t... Is>
//     auto push_front_helper(TTuple& t, Type&& new_data, std::index_sequence<Is...>) {
//         using old_fields = typename TTuple::fields;
//         using new_fields = map_push_front_t<old_fields, Tag, Type>;
//         // important: this moves unique pointers
//         return tagged_tuple_t<new_fields, typename TTuple::tags, typename TTuple::properties>(
//             ForwardToTupleConstructor(), std::forward<Type>(new_data),
//             copy_or_move(minimpl::box<std::tuple_element_t<Is, typename TTuple::tuple_t>>(),
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

// template <class Tag, class Fields, class Tags, class Properties>
// auto add_tag(tagged_tuple_t<Fields, Tags, Properties>& t) {
//     return tagged_tuple_t<Fields, minimpl::list_push_front_t<Tags, Tag>, Properties>(
//         ForwardToTupleConstructor(), std::move(t.data));
// }

// //==================================================================================================
// // adding props

// template <class Name, class Value, class Fields, class Tags, class Properties>
// auto add_prop(tagged_tuple_t<Fields, Tags, Properties>& t) {
//     return tagged_tuple_t<Fields, Tags, minimpl::map_push_front_t<Properties, Name, Value>>(
//         ForwardToTupleConstructor(), std::move(t.data));
// }

// //==================================================================================================
// // tagged_tuple type creation

// // to be used in field list declaration
// template <class Tag, class Type>
// using field = minimpl::pair<Tag, Type>;

// // alias used to construct ttuple type from a list of fields (actually a list of minimpl::pair)
// template <class... Fields>
// using tagged_tuple = tagged_tuple_t<minimpl::map<Fields...>>;

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
// using tag = minimpl::box<Tag>;

// template <class PropName, class PropValue>
// using property = minimpl::pair<PropName, PropValue>;

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
// using ttuple_has_tag = minimpl::list_contains<typename TTuple::tags, Tag>;

// // type of field
// template <class TTuple, class Tag>
// using field_type =
//     std::tuple_element_t<minimpl::map_element_index<typename TTuple::fields, Tag>::value,
//                          typename TTuple::tuple_t>;

// // get property
// template <class TTuple, class Tag>
// using get_property = minimpl::map_element_t<typename TTuple::properties, Tag>;
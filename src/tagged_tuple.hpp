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

#include "metadata.hpp"
#include "minimpl/src/type_map.hpp"
#include "ptr_utils.hpp"

template <class T, class U>
using field = type_pair<T, U>;

struct tuple_construct {};

template <class Metadata, class... Fields>
struct tagged_tuple {
    using field_map = type_map<Fields...>;
    using metadata = Metadata;
    map_value_list_t<field_map> data;

    tagged_tuple() = default;

    template <class... Args>
    tagged_tuple(tuple_construct, Args&&... args) : data(std::forward<Args>(args)...) {}
};

//==================================================================================================
template <class T>
using field_map_t = typename T::field_map;

//==================================================================================================
template <class T>
struct is_tagged_tuple : std::false_type {};

template <class MD, class... Fields>
struct is_tagged_tuple<tagged_tuple<MD, Fields...>> : std::true_type {};

//==================================================================================================
template <class Key, class MD, class... Fields>
auto& get(tagged_tuple<MD, Fields...>& t) {
    constexpr size_t index = map_element_index<Key, type_map<Fields...>>::value;
    return deref_if_ptr(get<index>(t.data));
}

template <class Key, class MD, class... Fields>
const auto& get(const tagged_tuple<MD, Fields...>& t) {
    constexpr size_t index = map_element_index<Key, type_map<Fields...>>::value;
    return deref_if_ptr(get<index>(t.data));
}

template <class FirstKey, class SecondKey, class... Rest, class MD, class... Fields>
auto& get(tagged_tuple<MD, Fields...>& t) {
    return get<SecondKey, Rest...>(get<FirstKey>(t));
}

template <class FirstKey, class SecondKey, class... Rest, class MD, class... Fields>
const auto& get(const tagged_tuple<MD, Fields...>& t) {
    return get<SecondKey, Rest...>(get<FirstKey>(t));
}

//==================================================================================================
namespace helper {
    template <class Tag, class Type, class MD, class... Fields, size_t... Is>
    auto push_front_impl(Type&& v, tagged_tuple<MD, Fields...>& t, std::index_sequence<Is...>) {
        using result_t = tagged_tuple<MD, field<Tag, std::remove_reference_t<Type>>, Fields...>;
        return result_t(tuple_construct(), std::move(v), std::move(get<Is>(t.data))...);
    }
};  // namespace helper

template <class Tag, class Type, class MD, class... Fields>
auto push_front(Type&& v, tagged_tuple<MD, Fields...>& t) {
    return helper::push_front_impl<Tag>(std::forward<Type>(v), t,
                                        std::make_index_sequence<sizeof...(Fields)>());
}

//==================================================================================================
// make tagged tuple from values

// to be used in make_tagged_tuple
template <class Tag, class Type>
struct TagValuePair {
    using field_type = type_pair<Tag, Type>;
    Type data;
    template <class InitType>  // might have different ref-ness than type
    explicit TagValuePair(InitType&& data) : data(std::forward<InitType>(data)) {}
    auto&& move() { return std::move(data); }
};

template <class Tag, class Type>
struct TagValuePair<Tag, Type&> {
    using field_type = type_pair<Tag, Type&>;
    Type& data;
    template <class InitType>  // might have different ref-ness than type
    explicit TagValuePair(InitType& data) : data(data) {}
    auto& move() { return data; }
};

// to be used in make_tagged_tuple calls
template <class Tag, class Type>
auto value_field(Type&& data) {
    using nonref_type = std::remove_reference_t<Type>;
    static_assert(
        std::is_copy_constructible<nonref_type>::value or std::is_rvalue_reference<Type&&>::value or
            not std::is_class<nonref_type>::value,
        "Trying to copy a non-copyable object into a field. Maybe you should move it instead.");
    return TagValuePair<Tag, nonref_type>{std::forward<Type>(data)};
}

// to be used in make_tagged_tuple calls
template <class Tag, class Type>
auto move_field(Type& data) {
    return TagValuePair<Tag, Type>{std::move(data)};
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

template <class MD = no_metadata, class... TVPairs>
auto make_tagged_tuple(TVPairs&&... pairs) {
    using result_t = tagged_tuple<MD, typename TVPairs::field_type...>;
    return result_t(tuple_construct(), pairs.move()...);
}

// //==================================================================================================
// // various type aliases to introspect tuple

// type of field
template <class Tag, class TTuple>
using field_type = map_element_t<Tag, field_map_t<TTuple>>;

template <class Tag, class MD, class... Fields>
constexpr bool has_tag(const tagged_tuple<MD, Fields...>&) {
    return metadata_has_tag<Tag, MD>::value;
}

template <class Tag, class MD, class... Fields>
constexpr bool has_property(const tagged_tuple<MD, Fields...>&) {
    return metadata_has_property<Tag, MD>::value;
}
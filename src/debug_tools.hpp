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

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include "tagged_tuple.hpp"

//==================================================================================================
// type id demangling

#ifdef __GNUG__
static std::string demangle(const char* name) {
    int status{0};
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status),
                                               std::free};
    return (status == 0) ? res.get() : name;
}
#else
static std::string demangle(const char* name) { return name; }
#endif

//==================================================================================================
// base interface for type-to-string

template <class T>
static std::string type_to_string();

template <class T>
static std::string type_to_string(T&) {
    return type_to_string<T>();
}

//==================================================================================================
// ttuple printer

namespace helper {
    template <class T>
    struct Type {};

    template <class TTuple>
    std::string ttuple_type_info_helper(std::tuple<>, Type<TTuple>) {
        return "";
    }

    template <class Tag, class Value, class... Rest, class TTuple>
    std::string ttuple_type_info_helper(std::tuple<utils::Pair<Tag, Value>, Rest...>,
                                        Type<TTuple>) {
        std::string type_str = type_to_string<Value>();
        std::string tag_str = type_to_string<Tag>();
        std::string other_fields = ttuple_type_info_helper(std::tuple<Rest...>(), Type<TTuple>());
        return type_str + " " + tag_str + "; " + other_fields;
    }

    template <class... Pairs>
    std::string ttuple_type_info_extract_types(Type<tagged_tuple_t<type_map::Map<Pairs...>>>) {
        using TTuple = tagged_tuple_t<type_map::Map<Pairs...>>;
        return "tagged_tuple { " +
               helper::ttuple_type_info_helper(std::tuple<Pairs...>(), helper::Type<TTuple>()) +
               "}";
    }

}  // namespace helper

//==================================================================================================
// handling references and unique pointers in a pretty way

namespace helper {
    template <class T>
    std::string pretty_refs_and_pointers(utils::Type<std::unique_ptr<T>>) {
        return "unique_ptr<" + demangle(typeid(T).name()) + ">";
    }

    template <class T>
    std::string pretty_refs_and_pointers(utils::Type<T&>) {
        return demangle(typeid(T).name()) + "&";
    }

    template <class T>
    std::string pretty_refs_and_pointers(utils::Type<T>) {
        return demangle(typeid(T).name());
    }
}  // namespace helper

//==================================================================================================
// selection of type-to-string impl (ttuple or not)

namespace helper {

    template <class T>
    std::string tagged_tuple_selector(std::true_type) {
        return helper::ttuple_type_info_extract_types(helper::Type<T>());
    }

    template <class T>
    std::string tagged_tuple_selector(std::false_type) {
        return helper::pretty_refs_and_pointers(utils::Type<T>());
    }
}  // namespace helper

template <class T>
static std::string type_to_string() {
    constexpr bool is_tuple = is_tagged_tuple<T>;
    return helper::tagged_tuple_selector<T>(std::integral_constant<bool, is_tuple>());
}

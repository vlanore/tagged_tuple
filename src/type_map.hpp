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

#include <tuple>
using std::tuple;

namespace type_map {
    // represents a pair of types
    template <class Tag, class Type>
    struct Pair {};

    // to be used as return value when key not found in map
    struct NotFound {};

    template <class... Decls>
    struct Map {
        template <class Value, int _index>
        // return of get_helper
        struct GetReturn {
            using value = Value;
            static constexpr int index = _index;
        };

        template <class Key, int index>
        // helper to recursively look for key in map (base case)
        static auto get_helper(tuple<>) {
            return NotFound();
        }

        template <class RequestedKey, int index, class Key, class Value, class... DeclRest>
        // helper to recursively look for key in map
        static auto get_helper(tuple<Pair<Key, Value>, DeclRest...>) {
            using if_equal = GetReturn<Value, index>;
            using if_not_equal =
                decltype(get_helper<RequestedKey, index + 1>(tuple<DeclRest...>()));
            constexpr bool equality = std::is_same<RequestedKey, Key>::value;
            return std::conditional_t<equality, if_equal, if_not_equal>();
        }  // note that return value is a default-constructed Value object (FIXME?)

        template <class Key>
        // type alias for the result of the get function
        using get = typename decltype(get_helper<Key, 0>(tuple<Decls...>()))::value;

        template <class Key>
        // type alias for the result of the get function
        static constexpr int get_index() {
            return decltype(get_helper<Key, 0>(tuple<Decls...>()))::index;
        }

        template <class Key, class Type>
        // type alias for the result of the add function
        using add = decltype(Map<Decls..., Pair<Key, Type>>());

        static auto value_tuple_helper(tuple<>) { return tuple<>(); }

        template <class Key, class Value, class... Rest>
        static auto value_tuple_helper(tuple<Pair<Key, Value>, Rest...>) {
            auto recursive_call = value_tuple_helper(tuple<Rest...>());
            return std::tuple_cat(tuple<Value>(), recursive_call);
        }

        using value_tuple_t = decltype(value_tuple_helper(tuple<Decls...>()));
    };
};  // namespace type_map

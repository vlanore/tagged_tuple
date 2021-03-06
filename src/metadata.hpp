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

struct no_metadata {};

template <class T>
using metadata_t = typename T::metadata;

template <class Name, class Value>
using property = type_pair<Name, Value>;

template <class Tags, class Properties>
using metadata = type_pair<Tags, Properties>;

template <class T>
using tags_t = first_t<T>;

template <class T>
using properties_t = second_t<T>;

template <class T>
struct is_metadata : std::false_type {};

template <class Tags, class Properties>
struct is_metadata<metadata<Tags, Properties>>
    : std::integral_constant<bool, is_list<Tags>::value and is_map<Properties>::value> {};

template <class Tag, class MD>
using metadata_has_tag = list_contains<Tag, tags_t<MD>>;

template <class Name, class MD>
using metadata_has_property = list_contains<Name, map_key_list_t<properties_t<MD>>>;

template <class Name, class MD>
using metadata_get_property = map_element_t<Name, properties_t<MD>>;

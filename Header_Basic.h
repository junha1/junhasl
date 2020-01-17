#pragma once

#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <array>
#include <unordered_map> 
#include <unordered_set> 
using std::vector;
using std::array;
using std::list;
using std::queue;
using std::set;
using std::map;
template <class K, class V>
using umap = std::unordered_map<K, V>;
template <class K>
using uset = std::unordered_set<K>;

#include <tuple>
using std::pair;
using std::tuple;
using std::make_pair;
using std::make_tuple;
using std::get;

#include <string>
#include <iostream>
using std::wstring;
using std::string;
using std::endl;

#include <memory>
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::const_pointer_cast;
using std::reinterpret_pointer_cast;
using std::make_shared;
using std::make_unique;

#include <functional>
using std::function;
using std::ref; using std::cref;
template<class T>
using refw = std::reference_wrapper<T>;

#include <optional>
#include <any>
#include <variant>
using std::optional;
using std::any;
using std::variant;

#include <type_traits>
using std::enable_if_t;
using std::is_same_v;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef long long int bint;
typedef unsigned long long int ubint;

typedef vector<uchar> bytes;
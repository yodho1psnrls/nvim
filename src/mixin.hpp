#pragma once

#include "meta_func.hpp"

#include <iostream>
#include <tuple>


template <typename CoreType>  // CRTP (PointCloud for example)
struct A {
  using wants = type_list<>;
  // using includes = type_list<TaggedType<int, vert_handle_tag>>;
  using includes = std::tuple<int>;
  
  void foo_a() const { std::cout << "A's value is " << std::get<int>(static_cast<const CoreType*>(this)->tup) << "\n"; }

  A() {
    std::get<int>(static_cast<CoreType*>(this)->tup) = 69;
  }
};

template <typename CoreType>  // CRTP (Mesh data structure for example)
struct B {
  using wants = type_list<>;
  // using includes = type_list<TaggedType<int, vert_handle_tag>>;
  using includes = std::tuple<char>;
  
  void foo_b() const { std::cout << "B's value is " << std::get<char>(static_cast<const CoreType*>(this)->tup) << "\n"; }

  B() {
    std::get<char>(static_cast<CoreType*>(this)->tup) = 'b';
  }
};

template <typename CoreType>  // CRTP (BVH over a Mesh for example)
struct C {
  using wants = type_list<>;
  // using includes = type_list<TaggedType<int, vert_handle_tag>>;
  using includes = std::tuple<float>;

  void foo_c() const { std::cout << "C's value is " << std::get<float>(static_cast<const CoreType*>(this)->tup) << "\n"; }

  C() {
    std::get<float>(static_cast<CoreType*>(this)->tup) = 11.11f;
  }
};

template <typename TupType> // (Holds the KeyKernel for example)
struct DataOnly {
  using tuple_type = TupType;
  tuple_type tup;
};

// template <typename ...Ts>
// struct DataHolder {
//  using tuple_type = std::tuple<Ts...>;
//  tuple_type _data;
// };

// NOTE: This is limited by the fact that all the CRTP Add-Ons should be
// templated with exactly one typename parameter

// template <typename ...Ts>
template <template <typename> class ...Ts> // The Mesh/Graph final data structure
class Mixin
// : DataHolder<tuple_cat_type_t<typename Ts::includes...>>
// : Ts... {
: public DataOnly<tuple_cat_type_t<typename Ts<Mixin<Ts...>>::includes...>>
, public Ts<Mixin<Ts...>>... {

  // using BaseType = DataHolder<tuple_cat_type_t<typename Ts::includes...>>;
  using ThisType = Mixin<Ts...>;
  using BaseTypes = type_list<Ts<Mixin<Ts...>>...>;
  using DataHolder = DataOnly<tuple_cat_type_t<typename Ts<Mixin<Ts...>>::includes...>>;

public:

  // using typename BaseType::tuple_type;
  // using BaseType::_data;
  
  // using tuple_type = tuple_cat_type_t<typename Ts<Mixin<Ts...>>::includes...>;
  // tuple_type tup;

  // Mixin() {}
  // Mixin() : DataHolder(), Ts<Mixin<Ts...>>()... {}

};

// TEST:
// using Mix = Mixin<A, B, C>;
// Mix mix;
// mix.foo_a();
// mix.foo_b();
// mix.foo_c();





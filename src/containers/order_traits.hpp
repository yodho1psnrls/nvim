#pragma once

#include "../utilities.hpp"
#include "../meta_func.hpp"
// #include "unordered_vector.hpp"  // We will forward declare it instead

#include <iterator>
#include <algorithm>
#include <type_traits>


struct ShiftOrder {

  template <typename IterType>
  static IterType to_right(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return std::shift_right(first, last, n);
  }
  
  template <typename IterType>
  static IterType to_left(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return std::shift_left(first, last, n);
  }
  
};


struct ChainOrder {

  template <typename IterType>
  static IterType to_right(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return chain_right(first, last, n);
  }
  
  template <typename IterType>
  static IterType to_left(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return chain_left(first, last, n);
  }
  
};


struct NoShiftOrder {

  template <typename IterType>
  static IterType to_right(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return first;
  }
  
  template <typename IterType>
  static IterType to_left(IterType first, IterType last,
    typename std::iterator_traits<IterType>::difference_type n = 1
  ) {
    return first;
  }
  
};


template <typename BaseVec>
struct UnorderedVector;


// template <typename VecType>
// struct is_unordered_vector
// : public std::false_type {};
//
// template <typename BaseVec>
// struct is_unordered_vector<UnorderedVector<BaseVec>>
// : public std::true_type {};
//
// template <typename VecType>
// constexpr bool is_unordered_vector_v = is_unordered_vector<VecType>::value;


template <typename VecType>
struct order_traits : public
  _if_t<
    is_vector_container_v<VecType>,
    ShiftOrder,
    NoShiftOrder
  > {};


template <typename BaseVec>
struct order_traits<
  UnorderedVector<BaseVec>
> : public ChainOrder {};



#pragma once

#include "../meta_func.hpp"  // safe_make_signed_t

#include <iterator>     // iterator categories and std::reverse_iterator
#include <type_traits>  // is_integral_v
#include <ostream>


template <typename NumType>
// template <typename IntType>
class IndexIterator {

  // static_assert(std::is_integral_v<NumType>);
  static_assert(std::is_arithmetic_v<NumType>);  // NOTE: Same as below
  // static_assert(
  //   std::is_integral_v<NumType> ||
  //   std::is_floating_point_v<NumType>
  // );

  NumType _id{};
  
  // template <typename>
  // friend class index_iterator;

public:

  // NOTE: Its not a contiguous iterator, because &(*(ii+n)) != &(*ii)+n (ii is index_iterator object)
  // using iterator_category = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  // using difference_type = std::make_signed_t<NumType>;
  using difference_type = safe_make_signed_t<NumType>;

  // using value_type = std::remove_const_t<IntType>;
  using value_type = NumType;
  using pointer = const value_type*;

  // using reference = const value_type&;
  // using reference = value_type;
  using reference = const value_type;


  constexpr IndexIterator() noexcept
  : _id() {}

  explicit constexpr IndexIterator(NumType id) noexcept
  : _id(id) {}

  template <typename I>
  constexpr IndexIterator(IndexIterator<I> other) noexcept
  : _id(*other) {}
  // : _id(other._id) {}

  // constexpr index_iterator& operator=(const index_iterator& other) = default;


  constexpr reference operator*() const noexcept { return _id; }  
  constexpr pointer operator->() const { return &_id; }
  constexpr reference operator[](difference_type i) const noexcept { return _id + i; }

  IndexIterator& operator++() { ++_id; return *this; }
  IndexIterator operator++(int) { IndexIterator temp(*this); ++(*this); return temp; }
  
  IndexIterator& operator--() { --_id; return *this; }
  IndexIterator operator--(int) { IndexIterator temp(*this); --(*this); return temp; }
  
  IndexIterator& operator+=(difference_type i) { _id += i; return *this; }
  IndexIterator operator+(difference_type i) const { IndexIterator temp(*this); return temp += i; }
  friend IndexIterator operator+(difference_type i, IndexIterator rhs) { return rhs += i; }

  IndexIterator& operator-=(difference_type i) { _id -= i; return *this; }
  IndexIterator operator-(difference_type i) const { IndexIterator temp(*this); return temp -= i; }
  difference_type operator-(IndexIterator other) const { return _id - other._id; }

  constexpr bool operator==(IndexIterator other) const { return _id == other._id; }
  constexpr bool operator!=(IndexIterator other) const { return _id != other._id; }
  constexpr bool operator>(IndexIterator other) const { return _id > other._id; }
  constexpr bool operator>=(IndexIterator other) const { return _id >= other._id; }
  constexpr bool operator<(IndexIterator other) const { return _id < other._id; }
  constexpr bool operator<=(IndexIterator other) const { return _id <= other._id; }

};

// template <typename I>
// inline index_iterator<I> operator+(std::make_signed_t<I> i, index_iterator<I> rhs) { return rhs += i; }

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, IndexIterator<T> id_it) {
  return stream << *id_it;
}
  


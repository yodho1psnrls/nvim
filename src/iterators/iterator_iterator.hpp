#pragma once

#include "../iterators/transform_iterator.hpp" // Need that only for the inverse versions
#include "../meta_func.hpp" // is_iterator_v

#include <iterator>     // iterator categories and std::reverse_iterator
#include <type_traits>  // std::enable_if


struct DereferenceFunctor {
  template <typename IT>
  std::iter_reference_t<IT>
  operator()(const IT& it) const {
    static_assert(is_iterator_v<IT>);
    return *it;
  }
};

template <typename IterType>
class IteratorIterator;


template <typename IterType>
inline IteratorIterator<IterType>
make_iterator_iterator(const IterType& it) {
  static_assert(is_iterator_v<IterType>);
  return IteratorIterator<IterType>(it);
}

template <typename IterType>
inline TransformIterator<IterType, DereferenceFunctor>
inverse_iterator_iterator(const IterType& it) {
  static_assert(is_iterator_v<IterType>);
  static_assert(is_iterator_v<std::iter_value_t<IterType>>);
  return TransformIterator<IterType, DereferenceFunctor>(it);
}

template <typename IterType>
inline IterType
inverse_iterator_iterator(const IteratorIterator<IterType>& it) {
  static_assert(is_iterator_v<IterType>);
  return it.base();
}


// Iterator Adaptor that returns the underlying iterator as a value
template <typename IterType>
class IteratorIterator {

  // NOTE: This is the only reason we need to include meta_func.hpp !!
  static_assert(is_iterator_v<IterType>);

  IterType _it;

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

public:

  using base_type = IterType;

  // using iterator_category = std::contiguous_iterator_tag;
  using iterator_category = typename std::iterator_traits<IterType>::iterator_category;
  using difference_type = typename std::iterator_traits<IterType>::difference_type;

  using value_type = IterType;
  using pointer = const IterType*;
  using reference = const IterType&;

  const base_type& base() const { return _it; }

  // template <bool COND = std::is_default_constructible_v<IterType>,
  //   typename = std::enable_if_t<COND>>
  // iterator_iterator() : _it() {}

  // explicit iterator_iterator(IterType it)
  explicit IteratorIterator(const IterType& it)
  : _it(it) {}
  
  template <typename OtherIter>
  IteratorIterator(const IteratorIterator<OtherIter>& other)
  : _it(other.base()) {}

  reference operator*() const { return _it; }  
  pointer operator->() const { return &_it; }
  value_type operator[](difference_type i) const { return _it + i; }

  IteratorIterator& operator++() { ++_it; return *this; }
  IteratorIterator operator++(int) { auto temp(*this); ++(*this); return temp; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC<IT>>>>
  IteratorIterator& operator--() { --_it; return *this; }
 
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC<IT>>>>
  IteratorIterator operator--(int) { auto temp(*this); --(*this); return temp; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  IteratorIterator& operator+=(difference_type i) { _it += i; return *this; }

  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  IteratorIterator operator+(difference_type i) const { auto temp(*this); return temp += i; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  friend IteratorIterator operator+(difference_type i, IteratorIterator rhs) { return rhs += i; }

  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  IteratorIterator& operator-=(difference_type i) { _it -= i; return *this; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  IteratorIterator operator-(difference_type i) const { auto temp(*this); return temp -= i; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  difference_type operator-(const IteratorIterator& other) const { return _it - other._it; }

  bool operator==(const IteratorIterator& other) const { return _it == other._it; }
  bool operator!=(const IteratorIterator& other) const { return _it != other._it; }
 
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  bool operator>(const IteratorIterator& other) const { return _it > other._it; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  bool operator>=(const IteratorIterator& other) const { return _it >= other._it; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  bool operator<(const IteratorIterator& other) const { return _it < other._it; }
  
  template <typename IT = IterType, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>>>
  bool operator<=(const IteratorIterator& other) const { return _it <= other._it; }

};

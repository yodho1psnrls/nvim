#pragma once
//#include "utilities.hpp"
#include "meta_func.hpp"
#include <type_traits>
#include <iterator>

// NOTE: There is std::const_iterator adaptor !!

// Use is_const_iterator
// so if it is a const iterator
// it just inherits it and its constructors and typedefs
// or even better, it just type aliases it directly
// , so it is the same exact type !

// Legacy Iterator
// https://en.cppreference.com/w/cpp/named_req/Iterator

// https://en.cppreference.com/w/cpp/iterator
// https://en.cppreference.com/w/cpp/iterator/basic_const_iterator

// https://isocpp.org/wiki/faq/const-correctness
// https://hackingcpp.com/cpp/design/east_vs_west_const.html


template <typename IterType>
class __const_iterator_wrapper_impl;


template <typename IterType>
using const_iterator_wrapper = _if_t<
  is_const_iterator<IterType>, 
  IterType,
  __const_iterator_wrapper_impl<IterType>
>;


// TEST
/*
  std::vector<int> vec = {13, 43, 56, 34, 67};
  std::cout << std::boolalpha;
  std::cout << std::is_same_v<
    decltype(vec)::const_iterator,
    const_iterator_wrapper<decltype(vec)::const_iterator>
  > << "\n";
  std::cout << std::boolalpha;
  std::cout << (std::is_same_v<
    const_iterator_wrapper<decltype(vec)::iterator>,
    const_iterator_wrapper<const_iterator_wrapper<decltype(vec)::iterator>>
  >) << "\n";
*/



template <typename IterType>
class __const_iterator_wrapper_impl {

  using ThisType = __const_iterator_wrapper_impl<IterType>;

  //template <typename IT = IterType>
  //using IfBidir = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<IT>::iterator_category>>;
  //template <typename IT = IterType>
  //using IfRand = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<IT>::iterator_category>>;

  IterType _it;

public:

  using iterator_category = std::iterator_traits<IterType>::iterator_category;
  using difference_type = std::iterator_traits<IterType>::difference_type;
  using value_type = std::iterator_traits<IterType>::value_type;

  static_assert(!std::is_same_v<iterator_category, std::output_iterator_tag>
    && "Output Iterators Are Forbidden !");

  // NOTE: I am not really sure if this wrapper can handle properly all the
  //        specifics of an std::input_iterator, so be careful with that

  // Those dont make sense, because they make the pointer or reference
  //  a const themselves, but still point to a non-const value
  //using reference = std::iterator_traits<IterType>::reference const;
  //using pointer = std::iterator_traits<IterType>::pointer const;

  using reference = const typename std::iterator_traits<IterType>::reference;
  using pointer = const typename std::iterator_traits<IterType>::pointer;

//  using reference = const value_type&;
//  using pointer = const value_type*;


  template <typename IT = IterType, typename = typename std::enable_if_t<std::is_default_constructible_v<IT>>>
  __const_iterator_wrapper_impl() : _it() {}

  __const_iterator_wrapper_impl(const IterType& it) : _it(it) {}

  const IterType& base() const { return _it; }

//  const_iterator_wrapper(const IterType& it) : _it(it) {}

  reference operator*() const { return *_it; }
  pointer operator->() const { return _it.operator->(); }


  ThisType& operator++() { ++_it; return *this; } 
  ThisType operator++(int) { auto temp(*this); ++_it; return temp; }

  //template <typename = typename IfBidir<IterType>>
  //ThisType& operator--() { ++_it; return *this; }

  bool operator==(const ThisType& other) const { return _it == other._it; }
  bool operator!=(const ThisType& other) const { return _it != other._it; }


// ---------------------- Legacy Bidirectional Iterator ------------------------- //


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::bidirectional_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType&>::type operator--() { --_it; return *this; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::bidirectional_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType>::type operator--(int) { auto temp(*this); --_it; return temp; }


// ---------------------- Legacy Random Access Iterator --------------------------- //


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType&>::type operator+=(const difference_type n) { _it += n; return *this; }
  
  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType&>::type operator-=(const difference_type n) { _it -= n; return *this; }


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType>::type operator+(const difference_type n) const { auto temp(*this); return temp += n; }

  
  template <typename IT = IterType>
  friend std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType>::type operator+(const difference_type n, const ThisType& cit) { return cit + n; }


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType>::type operator-(const difference_type n) const { auto temp(*this); return temp -= n; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  difference_type>::type operator-(const IT& other) const { return _it - other._it; }


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  reference>::type operator[](const difference_type n) const { return _it[n]; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  reference>::type at(const difference_type n) const { return _it[n]; }


  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  bool>::type operator<(const ThisType& other) const { return _it < other._it; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  bool>::type operator<=(const ThisType& other) const { return _it <= other._it; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  bool>::type operator>(const ThisType& other) const { return _it > other._it; }

  template <typename IT = IterType>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  bool>::type operator>=(const ThisType& other) const { return _it >= other._it; }


};



/*
Here are all the iterator categories defined
in the C++ Standard Library:

Input Iterator:
For read-only sequential access.
Example: std::istream_iterator.

Output Iterator:
For write-only sequential access.
Example: std::ostream_iterator.

Forward Iterator:
Read and write, single-pass, can traverse in one direction.
Example: std::forward_list.

Bidirectional Iterator:
Extends forward iterator with support for backward traversal.
Example: std::list, std::map.

Random Access Iterator:
Extends bidirectional iterator with constant-time indexing.
Example: std::vector, std::deque.

Contiguous Iterator (C++17 and later):
Extends random access with the guarantee that elements
are in a contiguous memory block.
Example: std::array, std::span.
*/



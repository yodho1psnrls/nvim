#pragma once

#include "../iterators/crtp_iterator.hpp"
#include "../iterators/index_iterator.hpp"
#include "../iterators/iterator_iterator.hpp"

#include <cstddef> // ptrdiff_t
#include <memory>
#include <type_traits> // ptrdiff_t
#include <iterator> // ptrdiff_t
#include <ostream>


struct index_handle_category{};
struct iter_handle_category{};


template <typename HandleType>
struct handle_traits {
  using handle_category = typename HandleType::handle_category;
  using handle_tag = typename HandleType::handle_tag;
  using iterator = typename HandleType::iterator;
};

// template <>
// struct handle_traits<std::ptrdiff_t> {
//   using handle_category = index_handle_tag;
// };



template <typename T, typename = void>
struct is_key_handle
: std::false_type {};

template <typename T>
struct is_key_handle<T, std::void_t<typename T::handle_category>>
// struct is_key_handle<T, std::void_t<typename handle_traits<T>::handle_category>>
: std::true_type {};

template <typename T>
constexpr bool is_key_handle_v = is_key_handle<T>::value;


// ================================================================ //


template <typename IntType = size_t, typename Tag = void>
class index_handle {

  using ThisType = index_handle<IntType, Tag>;

public:

  using handle_category = index_handle_category;
  using handle_tag = Tag;
  using iterator = index_iterator<IntType, ThisType>;

  using base_type = IntType;
  using difference_type = std::make_signed_t<IntType>;
  static constexpr difference_type INVALID_INDEX = -1;

  index_handle() : _id(INVALID_INDEX) {}
  explicit index_handle(base_type id) : _id(id) {}

  // template <typename Type, typename =
  //   std::enable_if_t<std::is_integral_v<Type>>>
  // explicit operator Type() const { return _id; }
  
  explicit operator IntType() const { return _id; }

  explicit operator index_handle<IntType, void>() const {
    return index_handle<IntType, void>(_id);
  }

  ThisType& operator++() { ++_id; return *this; }
  ThisType operator++(int) { ThisType temp(_id); ++(*this); return temp; }

  ThisType& operator--() { --_id; return *this; }
  ThisType operator--(int) { ThisType temp(_id); --(*this); return temp; }

  ThisType& operator+=(difference_type rhs) { _id += rhs; return *this; }
  ThisType operator+(difference_type rhs) const { ThisType temp(_id); return temp += rhs; }
  friend ThisType operator+(difference_type lhs, ThisType rhs) { return rhs += lhs; }

  ThisType& operator-=(difference_type rhs) { _id -= rhs; return *this; }
  ThisType operator-(difference_type rhs) const { ThisType temp(_id); return temp -= rhs; }
  friend difference_type operator-(ThisType lhs, ThisType rhs) { return lhs._id - rhs._id; }
  
  bool operator==(ThisType other) const { return _id == other._id; }
  bool operator!=(ThisType other) const { return _id != other._id; }
  bool operator>(ThisType other) const { return _id > other._id; }
  bool operator>=(ThisType other) const { return _id >= other._id; }
  bool operator<(ThisType other) const { return _id < other._id; }
  bool operator<=(ThisType other) const { return _id <= other._id; }

protected:

  base_type value() const {
    return _id;
  }

private:

  base_type _id;

};


/*template <typename IntType = size_t, typename Tag = void>
struct __index_handle_type {
  using type = __index_handle_impl<IntType, Tag>;
};

template <typename IntType>
struct __index_handle_type<IntType, void> {
  using type = IntType;
};

template <typename IntType = size_t, typename Tag = void>
using index_handle = typename __index_handle_type<IntType, Tag>::type;*/



template <typename IterType, typename Tag = void>
class iter_handle
: public CrtpIterator<iter_handle<Tag, IterType>, IterType> {
  using BaseType = CrtpIterator<iter_handle<Tag, IterType>, IterType>;
  using ThisType = iter_handle<IterType, Tag>;
public:
  using BaseType::BaseType;
  
  using handle_category = iter_handle_category;
  using handle_tag = Tag;
  // using iterator = iterator_iterator<IterType>;
  using iterator = iterator_iterator<ThisType>;

  using base_type = IterType;
  // using base_iter = IterType;
};


template <typename IntType, typename Tag>
std::ostream& operator<<(std::ostream& str, index_handle<IntType, Tag> id) {
  return str << IntType(id);
}

// template <typename Tag, typename IterType>
// std::ostream& operator<<(std::ostream& str, iter_handle<Tag, IterType> it) {
//   return str << *IterType(it);
// }


// ================================================================ //


struct vert_handle_tag {};
struct edge_handle_tag {};
struct face_handle_tag {};


template <typename IntType = std::ptrdiff_t>
using index_vert_handle = index_handle<IntType, vert_handle_tag>;

template <typename IntType = std::ptrdiff_t>
using index_edge_handle = index_handle<IntType, edge_handle_tag>;

template <typename IntType = std::ptrdiff_t>
using index_face_handle = index_handle<IntType, face_handle_tag>;


template <typename IterType>
using iter_vert_handle = iter_handle<IterType, vert_handle_tag>;

template <typename IterType>
using iter_edge_handle = iter_handle<IterType, edge_handle_tag>;

template <typename IterType>
using iter_face_handle = iter_handle<IterType, face_handle_tag>;



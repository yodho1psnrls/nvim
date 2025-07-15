#pragma once

#include "iterators/index_iterator.hpp"
#include "iterators/iterator_iterator.hpp"

#include "tagged.hpp"
#include "meta_func.hpp" // is_iterator_v

#include <iterator>
#include <type_traits>  // is_integral_v


struct vert_handle_tag {};  // Vertex Tag
struct edge_handle_tag {};  // Edge Tag
struct face_handle_tag {};  // Face(Facet) Tag
struct hedge_handle_tag {}; // Half-Edge Tag

// using vert_index = TaggedType<size_t, vert_handle_tag>;
// using edge_index = TaggedType<size_t, edge_handle_tag>;
// using face_index = TaggedType<size_t, face_handle_tag>;
// using hedge_index = TaggedType<size_t, hedge_handle_tag>;
//
// using vert_index_iterator = TaggedIterator<IndexIterator<size_t>, vert_handle_tag>;
// using edge_index_iterator = TaggedIterator<IndexIterator<size_t>, edge_handle_tag>;
// using face_index_iterator = TaggedIterator<IndexIterator<size_t>, face_handle_tag>;
// using hedge_index_iterator = TaggedIterator<IndexIterator<size_t>, hedge_handle_tag>;


struct index_handle_category {};
struct iterator_handle_category {};

template <typename HandleType, typename = void>
struct handle_traits;

template <typename T>
struct handle_traits<T, std::enable_if_t<std::is_integral_v<T>>> {
  using difference_type = std::make_signed_t<T>;
  using handle_category = index_handle_category;
  using iterator = IndexIterator<T>;
  // using handle_tag = void;
};

template <typename T>
struct handle_traits<T, std::enable_if_t<is_iterator_v<T>>> {
  using difference_type = typename std::iterator_traits<T>::difference_type;
  using handle_category = iterator_handle_category;
  using iterator = IteratorIterator<T>;
  // using handle_tag = void;
};

template <typename HandleType, typename Tag>
struct handle_traits<TaggedType<HandleType, Tag>>
: public handle_traits<HandleType> {
  using iterator = TaggedIterator<typename handle_traits<HandleType>::iterator, Tag>;
  using handle_tag = Tag;
};

// template <typename HandleType>
// struct is_key_handle {
//   static constexpr bool value =
//     std::is_integral_v<HandleType> ||
//     is_iterator_v<HandleType>;
// };
//
// template <typename HandleType, typename Tag>
// struct is_key_handle<TaggedType<HandleType, Tag>>
// : public is_key_handle<HandleType> {};

template <typename HandleType, typename = void>
struct is_key_handle
: public std::false_type {};

template <typename HandleType>
struct is_key_handle<
  HandleType,
  std::void_t<typename handle_traits<HandleType>::handle_category>
> : public std::true_type {};

template <typename HandleType>
constexpr bool is_key_handle_v = is_key_handle<HandleType>::value;



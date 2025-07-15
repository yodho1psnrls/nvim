#pragma once

#include "iterators/transform_iterator.hpp"
// #include "iterators/crtp_iterator.hpp"

#include <utility> // std::forward
#include <ostream>


// ------------------------------------------------------------------- //

template <typename T>
struct is_tag_type;

template <typename T>
constexpr bool is_tag_type_v = is_tag_type<T>::value;


template <typename T>
struct is_tagged_type;

template <typename T>
constexpr bool is_tagged_type_v = is_tagged_type<T>::value;


template <typename T, typename Tag>
class TaggedType {
  T _val;

  static_assert(!is_tagged_type_v<T>);
  static_assert(is_tag_type_v<Tag>);

public:

  using value_type = T;
  using tag_type = Tag;

  // explicit TaggedType(const T& val)
  // : _val(val) {}

  template <typename ...Args, typename = std::enable_if_t<
    std::is_constructible_v<T, Args...>>>
  explicit TaggedType(Args&&... args)
  : _val(std::forward<Args>(args)...) {}

  operator const T&() const {
    return _val;
  }

  // DANGER:
  operator T&() {
    return _val;
  }

  const T& base() const {
    return _val;
  }

  T& base() {
    return _val;
  }

};

template <typename T, typename Tag>
std::ostream& operator<<(std::ostream& os, const TaggedType<T, Tag>& tagged) {
  return os << static_cast<const T&>(tagged);
}

template <typename Tag>
struct MakeTaggedFunctor {
  template <typename T>
  TaggedType<T, Tag> operator()(const T& val) const {
    return static_cast<TaggedType<T, Tag>>(val);
  }
};

template <typename IT, typename Tag>
using TaggedIterator = TransformIterator<IT, MakeTaggedFunctor<Tag>>;

template <typename Tag, typename IterType>
inline TaggedIterator<IterType, Tag>
make_tagged_iterator(const IterType& it) {
  return TaggedIterator<IterType, Tag>(it);
}

/*template <typename KeyIter, typename Tag>
class TaggedIterator
: public CrtpIterator<TaggedIterator<KeyIter, Tag>, TransformIterator<KeyIter, MakeTaggedFunctor<Tag>>> {

  using ThisType = TaggedIterator<KeyIter, Tag>;
  using BaseType = CrtpIterator<ThisType, TransformIterator<KeyIter, MakeTaggedFunctor<Tag>>>;

public:

  using base_type = KeyIter;
  using tag_type = Tag;

  // template <typename ...Args, typename = std::enable_if_t<
  //   std::is_constructible_v<BaseType, Args...>>>
  // explicit TaggedIterator(Args&&... args)
  // : BaseType(std::forward<Args>(args)...) {}
  
  // explicit TaggedIterator(const KeyIter& key_it)
  // : BaseType(key_it) {}

  template <typename ...Args, typename =
    std::enable_if_t<std::is_constructible_v<KeyIter, Args...>>>
  explicit TaggedIterator(Args&&... args)
  : BaseType(std::forward<Args>(args)...) {}

  template <typename KI, typename = std::enable_if_t<
    std::is_constructible_v<KeyIter, KI>>>
  TaggedIterator(const TaggedIterator<KI, Tag>& other)
  : BaseType(other.iter()) {}


  // operator KeyIter() const {
  //   return BaseType::base().base();
  // }

  template <typename KI, typename = std::enable_if_t<
    std::is_constructible_v<KI, KeyIter>>>
  operator KI() const {
    return BaseType::iter().base();
  }

};*/

// ------------------------------------------------------------------- //

template <typename T>
struct is_tag_type {
  static constexpr bool value = std::is_empty_v<T>;
};


template <typename Tag>
struct get_tag_type {
  static_assert(is_tag_type_v<Tag>);
  using type = Tag;
};

template <typename T, typename Tag>
struct get_tag_type<TaggedType<T, Tag>> {
  using type = Tag;
};

template <typename Iter, typename Tag>
struct get_tag_type<TaggedIterator<Iter, Tag>> {
  using type = Tag;
};

template <typename T>
using get_tag_type_t = typename get_tag_type<T>::type;


template <typename T>
struct is_tagged_type
: public std::false_type {};

template <typename T, typename Tag>
struct is_tagged_type<TaggedType<T, Tag>>
: public std::true_type {};

template <typename T, typename Tag>
struct is_tagged_type<TaggedIterator<T, Tag>>
: public std::true_type {};



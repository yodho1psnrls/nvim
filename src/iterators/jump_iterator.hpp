#pragma once

#include "../meta_func.hpp"

#include <type_traits>  // std::is_constructible_v
#include <utility>      // std::forward

#include <iterator>     // iterator_traits
// #include <stdexcept>
#include <cassert>

// TODO: Hide the reference base() method of jump_iterator, ViewToIterator and TransformIterator
// , such that it is only accessed by GetIterBaseFunctor (that works with iterator adaptor/wrapper types
// to give you acess to the underlying base iterator).
// The GetAsIterFunctor used for the tensor iterator implementation, needs to access these
// bases too, so it will inherit from GetIterBaseFunctor and get the base from there only (if needed)

// TODO:
// Make the stackable feature only for jump_iterators !!!
// Make the tensor_view to exclusively be with a jump_iterator
// Make the ViewToIterator to mimic the jump_iterator interface


struct static_jump_policy_tag {};

struct fixed_jump_policy_tag
: public static_jump_policy_tag {};

struct nonfixed_jump_policy_tag
: public fixed_jump_policy_tag {};


class NoPolicy {
// template <typename Tag>
// template <int Index>
// class __NoPolicy {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

public:
  
  // https://stackoverflow.com/questions/67606956/standard-or-idiomatic-empty-object-in-c
  // static inline NoPolicy EMPTY = NoPolicy();

  using difference_type = std::ptrdiff_t;
  // using iterator_category = std::random_access_iterator_tag;
  using iterator_category_result = void;
  using required_iterator_category = void;
  using policy_category = static_jump_policy_tag;

  static constexpr std::ptrdiff_t value() { return 1; }

  explicit NoPolicy() = default;

public:

  // template <typename IT>
  // jump_iterator<IT, NoPolicy> get_iter(const IT& it) const {
  //   return jump_iterator<IT, NoPolicy>(it, *this);
  // }

  template <typename IT>
  static void advance(IT& it) {
    ++it;
  }
  
  template <typename IT>
  static void radvance(IT& it) {
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, IC<IT>>);
    --it;
  }
 
  template <typename IT>
  static void advance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += i;
  }
  
  template <typename IT>
  static void radvance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it -= i;
  }

  // template <typename JIT, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC<JIT>>>>
  // static difference_type get_diff(const JIT& lhs, const JIT& rhs) {
  //   static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<JIT>>);
  //   // return lhs.base() - rhs.base();
  //   return lhs.depth_base() - rhs.depth_base();
  // }

};


// Static version of FixedJumpPolicy
template <std::ptrdiff_t N>
// template <typename IntType, IntType N>
class StaticJumpPolicy {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

public:
  
  // https://stackoverflow.com/questions/67606956/standard-or-idiomatic-empty-object-in-c
  //static inline StaticJumpPolicy EMPTY = {};
  static inline StaticJumpPolicy EMPTY = StaticJumpPolicy<N>();
  // static inline __StaticJumpPolicy EMPTY = StaticJumpPolicy<N>();

  using difference_type = std::ptrdiff_t;
  // using iterator_category = std::random_access_iterator_tag;
  using iterator_category_result = std::random_access_iterator_tag;
  using required_iterator_category = std::random_access_iterator_tag;
  using policy_category = static_jump_policy_tag;

  // constexpr std::ptrdiff_t base() { return N; }
  // static constexpr StaticJumpPolicy& policy() { return EMPTY; }
  //static constexpr StaticJumpPolicy policy() { return StaticJumpPolicy(); }
  //static constexpr StaticJumpPolicy policy() { return {}; }

  static constexpr std::ptrdiff_t value() { return N; }

  explicit StaticJumpPolicy() = default;

// protected:
public:

  // template <typename IT>
  // jump_iterator<IT, StaticJumpPolicy> get_iter(const IT& it) const {
  //   return jump_iterator<IT, StaticJumpPolicy>(it, *this);
  // }

  template <typename IT>
  static void advance(IT& it) {
    static_assert(std::is_base_of_v<required_iterator_category, IC<IT>>);
    it += N;
  }
  
  template <typename IT>
  static void radvance(IT& it) {
    static_assert(std::is_base_of_v<required_iterator_category, IC<IT>>);
    it -= N;
  }
 
  template <typename IT>
  static void advance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<required_iterator_category, IC<IT>>);
    it += N * i;
  }
  
  template <typename IT>
  static void radvance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<required_iterator_category, IC<IT>>);
    it -= N * i;
  }

  // template <typename JIT>
  // static difference_type get_diff(const JIT& lhs, const JIT& rhs) {
  //   static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<JIT>>);
    // if constexpr (N)
    //  // return (lhs.base() - rhs.base()) / N;
    //  return (lhs.depth_base() - rhs.depth_base()) / N;
    // return 0;
  // }

};

template <>
struct StaticJumpPolicy<1>
: public NoPolicy {

  using NoPolicy::NoPolicy;
 
  StaticJumpPolicy(const NoPolicy& other)
  : StaticJumpPolicy() {}

};


template <typename T>
class FixedJumpPolicy {

  static_assert(std::is_arithmetic_v<T>);

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;
  
  T _n;  // stride(jump) length

public:

  // using difference_type = T;
  // using difference_type = std::make_signed_t<T>;
  using difference_type = safe_make_signed_t<T>;
  // using iterator_category = std::random_access_iterator_tag;
  using iterator_category_result = std::random_access_iterator_tag;
  using required_iterator_category = std::random_access_iterator_tag;
  using policy_category = fixed_jump_policy_tag;
  
  //difference_type stride() const { return _n; }
  // std::ptrdiff_t base() const { return _n; }
  // const FixedJumpPolicy& policy() const { return *this; }
  difference_type value() const { return _n; }
  // FixedJumpPolicy() = default;
  FixedJumpPolicy(difference_type stride) : _n(stride) {}
  // __FixedJumpPolicy(difference_type stride) : _n(stride) {}
  
  template <typename OT>
  FixedJumpPolicy(const FixedJumpPolicy<OT> other)
  : _n(other.value()) {
    static_assert(std::is_convertible_v<OT, T>);
  }
  
  //template <typename IT>
  //FixedJumpPolicy(const jump_iterator<IT, FixedJumpPolicy>&) = delete;

// protected:
public:

  // template <typename IT>
  // jump_iterator<IT, FixedJumpPolicy> get_iter(const IT& it) const {
  //   return jump_iterator<IT, FixedJumpPolicy>(it, *this);
  // }
 
  template <typename IT>
  void advance(IT& it) const {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += _n;
  }
  
  template <typename IT>
  void radvance(IT& it) const {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it -= _n;
  }
 
  template <typename IT>
  void advance(IT& it, difference_type i) const {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += _n * i;
  }
  
  template <typename IT>
  void radvance(IT& it, difference_type i) const {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it -= _n * i;
  }

  /*template <typename JIT>
  static difference_type get_diff(const JIT& lhs, const JIT& rhs) {
    if (lhs.stride())
      return (lhs.depth_base() - rhs.depth_base()) / lhs.stride();
    return 0;

    // NOTE: Branchless check for division by zero doesnt work, the thread still stops
    // bool COND = lhs.value() != 0;
    // return COND * (lhs.base() - rhs.base()) / lhs.value() + !COND * 0;
    // return bool(lhs.value()) * ((lhs.base() - rhs.base()) / lhs.value());
  }*/

};


// ================================================================================= //

template <typename IndexIter, bool Cyclic = false>
class JumpByIndexValue {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;
  
  static_assert(is_iterator_v<IndexIter>);
  // static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);
  static_assert(std::is_arithmetic_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  // using iterator_category = base_t<IC<IndexIter>, std::bidirectional_iterator_tag>;
  using iterator_category_result = base_t<IC<IndexIter>, std::bidirectional_iterator_tag>;
  using required_iterator_category = std::random_access_iterator_tag;
  using policy_category = nonfixed_jump_policy_tag;

  // JumpByIndexValue() = default;
  JumpByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}
  // __JumpByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}
  
  template <typename IT>
  JumpByIndexValue(const JumpByIndexValue<IT, Cyclic>& other)
  : _id_it(other.iter()) {
    static_assert(std::is_convertible_v<IT, IndexIter>);
  }

  using iter_type = IndexIter;
  iter_type& iter() { return _id_it; }
  const iter_type& iter() const { return _id_it; }

  // const IndexIter& base() const { return _id_it; }
  difference_type value() const { return *_id_it; }
  // const JumpByIndexValue& policy() const { return *this; }
  
  //template <typename IT>
  //JumpByIndexValue(const jump_iterator<IT, JumpByIndexValue>&) = delete;

// protected:
public:

  // template <typename IT>
  // jump_iterator<IT, JumpByIndexValue> get_iter(const IT& it) const {
  //   return jump_iterator<IT, JumpByIndexValue>(it, *this);
  // }
 
  void advance_alone() { ++_id_it; }
  void radvance_alone() { --_id_it; }
  void advance_alone(difference_type i) { _id_it += i; }
  void radvance_alone(difference_type i) { _id_it -= i; }

 //  template <typename IT>
 //  IT next(const IT&) { return *std::next(_id_it); }
 //  
 //  template <typename IT>
 //  IT prev(const IT&) { return *std::prev(_id_it); }
 //  
 //  template <typename IT>
 //  IT next(const IT&, difference_type i) { return *std::next(_id_it, i); }
 // 
 //  template <typename IT>
 //  IT prev(const IT&, difference_type i) { return *std::prev(_id_it, i); }


  template <typename IT>
  // template <typename IT, typename = std::enable_if_t<is_iterator_v<IT>>>
  void advance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += *(_id_it++);
  }
  
  template <typename IT>
  void radvance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it -= *(--_id_it);
  }

};


template <typename IndexIter>
class JumpByIndexDiff {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;
  
  static_assert(is_iterator_v<IndexIter>);
  // static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);
  static_assert(std::is_arithmetic_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
 
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  // using iterator_category = typename std::iterator_traits<IndexIter>::iterator_category;
  using iterator_category_result = typename std::iterator_traits<IndexIter>::iterator_category;
  using required_iterator_category = std::random_access_iterator_tag;
  using policy_category = nonfixed_jump_policy_tag;
  
  using iter_type = IndexIter;
  iter_type& iter() { return _id_it; }
  const iter_type& iter() const { return _id_it; }

  // const IndexIter& base() const { return _id_it; }
  // const JumpByIndexDiff& policy() const { return *this; }
  difference_type value() const { return *(std::next(_id_it)) - *_id_it; }
  // JumpByIndexDiff() = default;
  JumpByIndexDiff(const IndexIter& index_id) : _id_it(index_id) {}
  // __JumpByIndexDiff(const IndexIter& index_id) : _id_it(index_id) {}
  
  template <typename IT>
  JumpByIndexDiff(const JumpByIndexDiff<IT>& other)
  : _id_it(other.iter()) {
    static_assert(std::is_convertible_v<IT, IndexIter>);
  }

  //template <typename IT>
  //JumpByIndexDiff(const jump_iterator<IT, JumpByIndexDiff>&) = delete;

public:
  
  void advance_alone() { ++_id_it; }
  void radvance_alone() { --_id_it; }
  void advance_alone(difference_type i) { _id_it += i; }
  void radvance_alone(difference_type i) { _id_it -= i; }
 
  template <typename IT>
  void advance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    auto id0 = *_id_it;
    it += *(++_id_it) - id0;
  }

  template <typename IT>
  void radvance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    auto id0 = *_id_it;
    it += *(--_id_it) - id0;
  }

  template <typename IT>
  void advance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    auto id0 = *_id_it;
    it += *(_id_it += i) - id0;
  }

  template <typename IT>
  void radvance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    auto id0 = *_id_it;
    it += *(_id_it -= i) - id0;
  }

  /*template <typename JIT>
  static difference_type get_diff(const JIT& lhs, const JIT& rhs) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<JIT>>);
    // return lhs.policy()._id_it - rhs.policy()._id_it;
    return lhs.policy().base() - rhs.policy().base();
  }*/

};


// This Policy allows you you use jump iterator with
//  non-random access iterators
// It might seem redundant at first, but this policy
//  could be used as stride policy for tensor iterator
//  or size policy for tensor iterable
// Allowing classes like std::list to be the flat base
//  for JaggedMatrix and Tensors
template <typename IterIter>
class JumpByEqualNext {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;
  
  template <typename IT>
  using VT = typename std::iterator_traits<IT>::value_type;

  static_assert(is_iterator_v<IterIter>);
  // static_assert(is_iterator_v<typename std::iterator_traits<IterIter>::value_type>);

  IterIter _it_it;

public:
  
  using difference_type = typename std::iterator_traits<IterIter>::difference_type;
  // using iterator_category = typename std::iterator_traits<IterIter>::iterator_category;
  using iterator_category_result = typename std::iterator_traits<IterIter>::iterator_category;
  using required_iterator_category = std::random_access_iterator_tag;
  using policy_category = nonfixed_jump_policy_tag;

  using iter_type = IterIter;
  iter_type& iter() { return _it_it; }
  const iter_type& iter() const { return _it_it; }

  difference_type value() const { return *(std::next(_it_it)) - *_it_it; }

  JumpByEqualNext(const IterIter& iter_it) : _it_it(iter_it) {}
  
  template <typename IT>
  JumpByEqualNext(const JumpByEqualNext<IT>& other)
  : _it_it(other.iter()) {
    static_assert(std::is_convertible_v<IT, IterIter>);
  }

public:
  
  void advance_alone() {
    ++_it_it;
  }
  void radvance_alone() {
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category_result>);
    --_it_it;
  }
  void advance_alone(difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
    _it_it += i;
  }
  void radvance_alone(difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
    _it_it -= i;
  }
 
  template <typename IT>
  void advance(IT& it) {
    // static_assert(is_iterator_v<IT>);
    static_assert(std::is_convertible_v<VT<IterIter>, IT>);

    ++_it_it;
    it = *_it_it;
    // it.base() = IterIter(*_it_it);
  }

  template <typename IT>
  void radvance(IT& it) {
    // static_assert(is_iterator_v<IT>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category_result>);
    static_assert(std::is_convertible_v<VT<IterIter>, IT>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, IC<IterIter>>);

    --_it_it;
    it = *_it_it;
    // it.base() = IterIter(*_it_it);
  }

  template <typename IT>
  void advance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
    static_assert(std::is_convertible_v<VT<IterIter>, IT>);
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IterIter>>);
    _it_it += i;
    it = *_it_it;
    // it.base() = IterIter(*_it_it);
  }

  template <typename IT>
  void radvance(IT& it, difference_type i) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
    static_assert(std::is_convertible_v<VT<IterIter>, IT>);
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IterIter>>);
    _it_it -= i;
    it = *_it_it;
    // it.base() = IterIter(*_it_it);
  }

  /*template <typename JIT>
  static difference_type get_diff(const JIT& lhs, const JIT& rhs) {
    // static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IterIter>>);
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category>);
    // return lhs.policy()._it_it - rhs.policy()._it_it;
    return lhs.policy().base() - rhs.policy().base();
  }*/

  // Examples:
  //   std::list l = {'a', 'b', 'c', 'd', 'e', 'f'};
  //   std::vector/list iters = {l.begin(), std::next(l.begin(), 2), std::next(l.begin(), 4), l.end()};
  //   auto itb = make_tensor_iterable(
  //     l.begin(),
  //     iters.begin(),
  //     iters.size() - 1
  //   );
  //   auto itb = make_tensor_iterable_ext(
  //     l.begin(), NoPolicy(),
  //     iters.begin(), UseItbSize(),
  //     std::prev(iters.end(), 2)
  //   );

};


// NOTE: Cannot do that, because the constructor of jump_iterator
// should be changed to do the jump until satisfactory condition
/*template <typename BoolIter>
class JumpByCondition {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;
  
  template <typename IT>
  using VT = typename std::iterator_traits<IT>::value_type;

  static_assert(is_iterator_v<BoolIter>);
  static_assert(std::is_convertible_v<typename std::iterator_traits<BoolIter>::reference, bool>);

  BoolIter _b_it;

public:
  
  using difference_type = typename std::iterator_traits<BoolIter>::difference_type;
  using iterator_category_result = std::bidirectional_iterator_tag;
  using required_iterator_category = std::bidirectional_iterator_tag;
  using policy_category = nonfixed_jump_policy_tag;

  using iter_type = BoolIter;
  iter_type& iter() { return _b_it; }
  const iter_type& iter() const { return _b_it; }

  difference_type value() const {
    difference_type n = 0;
    BoolIter temp(_b_it);
    do {
      ++n;
      ++temp;
    } while(*temp);
    return n;
  }

  JumpByCondition(const BoolIter& iter_it)
  : _b_it(iter_it) {}
  
  template <typename IT>
  JumpByCondition(const JumpByCondition<IT>& other)
  : _b_it(other.iter()) {
    static_assert(std::is_constructible_v<BoolIter, IT>);
  }

public:
  
  // void advance_alone() {
  //   ++_b_it;
  // }
  // void radvance_alone() {
  //   static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category_result>);
  //   --_b_it;
  // }
  // void advance_alone(difference_type i) {
  //   static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
  //   _b_it += i;
  // }
  // void radvance_alone(difference_type i) {
  //   static_assert(std::is_base_of_v<std::random_access_iterator_tag, iterator_category_result>);
  //   _b_it -= i;
  // }
 
  template <typename IT>
  void advance(IT& it) {
    static_assert(is_iterator_v<IT>);

    do {
      ++it;
      ++_b_it;
    } while(*_b_it);
  }

  template <typename IT>
  void radvance(IT& it) {
    static_assert(is_iterator_v<IT>);

    do {
      --it;
      --_b_it;
    } while(*_b_it);
  }

  // template <typename IT>
  // void advance(IT& it, difference_type i) {
    // static_assert(is_iterator_v<IT>);
  // }
  //
  // template <typename IT>
  // void radvance(IT& it, difference_type i) {
    // static_assert(is_iterator_v<IT>);
  // }

};*/


// =================================================================== //

// TODO: Fix
// std::vector vec = {1, 2, 3};
// // jump_iterator jit(vec.cbegin());
// // jit = vec.begin();
// jump_iterator jit(vec.begin());
// decltype(vec.cbegin()) it = jit;


template <
  typename IterType,
  typename Policy,
  size_t DEPTH = 1
  // bool STACKABLE = true
>
class JumpIterator;


template <typename T>
struct is_jump_iterator
: public std::false_type {};

template <typename IT, typename P, size_t DEPTH>
struct is_jump_iterator<JumpIterator<IT, P, DEPTH>>
: public std::true_type {};

template <typename T>
constexpr bool is_jump_iterator_v = is_jump_iterator<T>::value;


template <typename T, typename = void>
struct is_jump_policy
: public std::false_type {};

template <typename Policy>
struct is_jump_policy<
  Policy,
  // std::void_t<typename Policy::policy_category>
  std::void_t<
    typename Policy::policy_category,
    typename Policy::required_iterator_category,
    typename Policy::iterator_category_result,
    typename Policy::difference_type
  >
> : public std::true_type {};

template <typename T>
constexpr bool is_jump_policy_v = is_jump_policy<T>::value;


/*template <typename BaseIter, bool STACKABLE>
struct __get_depth_base_impl;

template <typename BaseIter>
struct __get_depth_base_impl<BaseIter, true> {
  using type = BaseIter;
};

template <typename BaseIter>
struct __get_depth_base_impl<BaseIter, false> {
  using type = typename BaseIter::depth_base_type;
};

template <typename BaseIter, bool STACKABLE>
using __get_depth_base_t = typename __get_depth_base_impl<BaseIter, STACKABLE>::type;*/

// template <typename IterType>
// inline auto&& get_iter_base(IterType&& iter, const size_t N = 1) {
//  if (N)
//    return get_iter_base(iter.base(), N - 1);
//  return iter;
// }

template <size_t DEPTH = 1, typename T>
inline auto&& get_depth_base(T&& val) {
  // static_assert(std::is_same_v<typename T::base_type&, decltype(std::declval<T>().base())>);
  if constexpr (DEPTH == 0)
    return std::forward<T>(val);
  else if constexpr (DEPTH == 1)
    return std::forward<T>(val).base();
  // esle if constexpr (DEPTH == 2)
  //  return val.base().base();
  // else if constexpr (DEPTH == 3)
  //  return val.base().base().base();
  else
    return get_depth_base<DEPTH - 1>(std::forward<T>(val).base());
}


template <typename IterType, size_t DEPTH = 1>
using get_base_typedef_t = std::remove_cvref_t<decltype(get_depth_base<DEPTH>(std::declval<IterType>()))>;


template <typename IterType, size_t DEPTH = 1, typename = void>
class does_have_ref_base_method
: public std::false_type {};

template <typename IterType>
class does_have_ref_base_method<IterType, 0, void>
: public std::true_type {};

template <typename IterType, size_t DEPTH>
class does_have_ref_base_method<
  IterType,
  DEPTH,
  std::void_t<decltype(std::declval<IterType>().base())>
> {

  using result = decltype(std::declval<IterType>().base());

public:

  static constexpr bool value =
    std::is_reference_v<result> &&
    !std::is_const_v<std::remove_reference_t<result>>
  && does_have_ref_base_method<result, DEPTH - 1>::value;

};

template <typename IterType, size_t DEPTH = 1>
constexpr bool does_have_ref_base_method_v = does_have_ref_base_method<IterType, DEPTH>::value;


// ============================================================================================ //


template <typename IterType, typename Policy, size_t DEPTH>
class JumpIterator {

  static_assert(is_iterator_v<IterType>);
  static_assert(is_jump_policy_v<Policy>);
  static_assert(DEPTH != 0);

  // static_assert(does_have_base_typedef_v<IterType, DEPTH - 1>);
  // static_assert(does_have_base_method_v<IterType, DEPTH - 1>);
  static_assert(does_have_ref_base_method_v<IterType, DEPTH - 1>);

  static constexpr bool NONFIXED_STRIDE = std::is_base_of_v<nonfixed_jump_policy_tag, typename Policy::policy_category>;

  using ThisType = JumpIterator<IterType, Policy, DEPTH>;

public:
  
  // using depth_base_type = get_base_type_t<IterType, !STACKABLE>; // 1 gets the base true is 1 false is 0
  using depth_base_type = get_base_typedef_t<IterType, DEPTH - 1>;
  // static constexpr size_t NUM_DEPTH = DEPTH;
  
  using base_type = IterType;
  using policy_type = Policy;

  // If there is a required iterator category for the base iter, asserted from the policy
  static_assert(
    !!std::is_void_v<typename policy_type::required_iterator_category>
    || std::is_base_of_v<
      typename policy_type::required_iterator_category,
      typename std::iterator_traits<depth_base_type>::iterator_category
  >);

  using difference_type = typename policy_type::difference_type;

  // using iterator_category = typename policy_type::iterator_category;
  // using iterator_category = typename policy_type::iterator_category_result;
  using iterator_category = _if_t<
    // std::is_same_v<void, typename policy_type::iterator_category_result>,
    std::is_void_v<typename policy_type::iterator_category_result>,
    typename std::iterator_traits<depth_base_type>::iterator_category,
    typename policy_type::iterator_category_result
  >;

  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;

  const policy_type& policy() const noexcept {
    // return static_cast<const policy_type&>(*this);
    return _pol;
  }
  
  policy_type& policy() noexcept {
    // return static_cast<policy_type&>(*this);
    return _pol;
  }

  difference_type stride() const {
    // return policy_type::value();
    return _pol.value();
    // if constexpr (DEPTH == 1)
    //  return _pol.value();
    // else
    //  return 
  }


private:

  IterType _it;
  [[no_unique_address]] Policy _pol;

protected:

  // template <typename, typename, size_t>
  // friend class jump_iterator;

  // NOTE: We dont need this any more
  // friend Policy; // Because of get_diff(...)
  
  depth_base_type& depth_base() noexcept {
    if constexpr (DEPTH == 1)
      return _it;
    else
      return get_depth_base<DEPTH - 1>(_it);
  }

  const depth_base_type& depth_base() const noexcept {
    if constexpr (DEPTH == 1)
      return _it;
    else
      return get_depth_base<DEPTH - 1>(_it);
  }
  
// TODO: Make the non-const returning base() to be protected,
//  and acessed only by the get_depth_base helper function
// protected:
//  template <typename T>
//  friend auto&& get_depth_base(T&& val);
//  base_type& base() noexcept { return _it; }

public:

  base_type& base() noexcept { return _it; }
  const base_type& base() const noexcept { return _it; }

  template <typename P = Policy, typename =
    std::enable_if_t<std::is_default_constructible_v<P>>>
  [[nodiscard]] JumpIterator(const IterType& iter) noexcept 
  : _it(iter), _pol() {}

  // Why this constructor doesnt call with an implicit
  //  conversion from a single argument of the policy constructor ?
  // Because you may be giving iterator when the argument is 
  //  const iterator, so it gets a very long ching of conversions
  //  iterator -> const_iterator -> Policy<const_iterator>
  //  that can not be implicitly deduced
  // And thats why we would just have the helper constructor
  //  above this
  [[nodiscard]] JumpIterator(const IterType& iter, const Policy& stride) noexcept
   : _it(iter), _pol(stride) {}
  
  // template <typename P = Policy, typename =
  //  std::enable_if_t<std::is_constructible_v<policy_type, P>>>
  // jump_iterator(const IterType& iter, const P& stride)
  //   : _it(iter), _pol(stride) {}
  
  template <typename IT, typename = std::enable_if_t<
    std::is_constructible_v<IterType, IT>>>
  [[nodiscard]] JumpIterator(const JumpIterator<IT, Policy, DEPTH>& other) noexcept
    : _it(other.base()), _pol(other.policy()) {}
  
  // template <typename IT, typename P, typename = std::enable_if_t<
  //   std::is_constructible_v<IterType, IT> && std::is_constructible_v<Policy, P>>>
  // jump_iterator(const jump_iterator<IT, P, DEPTH>& other) noexcept
  //   : _it(other.base()), _pol(other.policy()) {}


  // NOTE: Should this be allowed ?
  // template <typename P = Policy, typename =
  //   std::enable_if_t<DEPTH == 1 && std::is_base_of_v<NoPolicy, P>>>
  // operator const IterType& () const noexcept { return _it; }
  // 
  // template <typename P = Policy, typename =
  //   std::enable_if_t<DEPTH == 1 && std::is_base_of_v<NoPolicy, P>>>
  // operator IterType& () noexcept { return _it; }


  reference operator*() const { return *_it; }
  pointer operator->() const { return _it.operator->(); }

  template <typename IC = iterator_category, typename =
    std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference operator[](difference_type i) const {
    ThisType temp(*this); 
    return *(temp += i);
  }


  ThisType& operator++() { _pol.advance(depth_base()) ; return *this; }
  ThisType operator++(int) { ThisType temp(*this); ++(*this); return temp; }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  ThisType& operator--() { _pol.radvance(depth_base()); return *this; }
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  ThisType operator--(int) { ThisType temp(*this); --(*this); return temp; }


  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType& operator+=(difference_type i) { _pol.advance(depth_base(), i); return *this; }
  // friend ThisType operator+(ThisType iter, difference_type i) { return iter += i; }
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType operator+(difference_type i) const { ThisType temp(*this); return temp += i; }
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend ThisType operator+(difference_type i, ThisType iter) { return iter += i; }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType& operator-=(difference_type i) { _pol.radvance(depth_base(), i); return *this; }
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend ThisType operator-(ThisType iter, difference_type i) { return iter -= i; }
  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  difference_type operator-(const ThisType& rhs) const {
    // return _pol.get_diff(*this, rhs);
    if constexpr (NONFIXED_STRIDE)
      return _pol.iter() - rhs._pol.iter();
    else {
      if (_pol.value())
        return (depth_base() - rhs.depth_base()) / _pol.value();
      return 0;
    }
  }


  bool operator==(const JumpIterator& other) const {
    // NOTE: May be needed
    // if constexpr (std::is_base_of_v<std::random_access_iterator_tag, iterator_category>)
    //   return !(*this - other);
    // else
    return depth_base() == other.depth_base();
  }

  bool operator!=(const JumpIterator& other) const {
    // NOTE: May be needed
    // if constexpr (std::is_base_of_v<std::random_access_iterator_tag, iterator_category>)
    //   return *this - other;
    // else
    return depth_base() != other.depth_base();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<(const JumpIterator& other) const {
    // return depth_base() < other.depth_base();
    return (*this - other) < 0;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<=(const JumpIterator& other) const {
    // return depth_base() <= other.depth_base();
    return (*this - other) <= 0;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>(const JumpIterator& other) const {
    // return depth_base() > other.depth_base();
    return (*this - other) > 0;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>=(const JumpIterator& other) const {
    // return depth_base() >= other.depth_base();
    return (*this - other) >= 0;
  }

  // ========================== SENTINELS ============================= //

  /*bool operator==(const depth_base_type& other) const {
    return depth_base() == other;
  }

  bool operator!=(const depth_base_type& other) const {
    return depth_base() != other;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC> && !NONFIXED_STRIDE>>
  bool operator<(const depth_base_type& other) const {
    return depth_base() < other;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<=(const depth_base_type& other) const {
    return depth_base() <= other;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>(const depth_base_type& other) const {
    return depth_base() > other;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>=(const depth_base_type& other) const {
    return depth_base() >= other;
  }


  friend bool operator==(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs == rhs.depth_base();
  }

  friend bool operator!=(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs != rhs.depth_base();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC> && !NONFIXED_STRIDE>>
  friend bool operator<(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs < rhs.depth_base();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend bool operator<=(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs <= rhs.depth_base();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend bool operator>(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs > rhs.depth_base();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend bool operator>=(const depth_base_type& lhs, const JumpIterator& rhs) {
    return lhs >= rhs.depth_base();
  }*/

};

template <
  typename PolicyArg, 
  bool IS_ITER = is_iterator_v<std::remove_cvref_t<PolicyArg>>
>
class arg_to_jump_policy;

template <typename PolicyArg>
class arg_to_jump_policy<PolicyArg, false> {
  using PA = std::remove_cvref_t<PolicyArg>;
public:
  using type = _if_t<
    // std::is_integral_v<PA>,
    std::is_arithmetic_v<PA>,
    FixedJumpPolicy<PA>,
    PA
  >;
};

template <typename PolicyArg>
class arg_to_jump_policy<PolicyArg, true> {
  using PA = std::remove_cvref_t<PolicyArg>;
public:
  using type =
    // _if_t<
    //  std::is_same_v<bool, typename std::iterator_traits<PA>::value_type>,
    //  JumpByCondition<PA>,
      _if_t<
        std::is_integral_v<typename std::iterator_traits<PA>::value_type>,
        JumpByIndexDiff<PA>,
        _if_t<
          is_iterator_v<typename std::iterator_traits<PA>::value_type>,
          JumpByEqualNext<PA>,
          // void
          PA
        >
    // >
  >;
};


template <typename PolicyArg>
using arg_to_jump_policy_t = typename arg_to_jump_policy<PolicyArg>::type;


/*template <typename JumpIter, long long DIFF>
struct add_depth_to_jump_iterator;

template <typename IterType, typename Policy, size_t DEPTH, long long DIFF>
struct add_depth_to_jump_iterator<jump_iterator<IterType, Policy, DEPTH>, DIFF> {
  using type = jump_iterator<IterType, Policy, DEPTH + DIFF>;
};

template <typename JumpIter, long long DIFF>
using add_depth_to_jump_iterator_t = typename add_depth_to_jump_iterator<JumpIter, DIFF>::type;*/


/*template <typename JumpIter, bool COND = is_jump_iterator_v<typename JumpIter::depth_base_type>>
struct __recursive_jump_iterator_impl {};

template <typename IterType, typename Policy, size_t DEPTH>
class __recursive_jump_iterator_impl<jump_iterator<IterType, Policy, DEPTH>, true> {
  using DB = typename jump_iterator<IterType, Policy, DEPTH>::depth_base_type;
public:
  using type = jump_iterator<IterType, Policy, DEPTH + DB::NUM_DEPTH>;
};

template <typename IterType, typename Policy, size_t DEPTH>
struct __recursive_jump_iterator_impl<jump_iterator<IterType, Policy, DEPTH>, false> {
  using type = jump_iterator<IterType, Policy, DEPTH>;
};*/


template <typename IterType>
struct get_iter_depth {
  // NOTE: What if this is 1 ?
  // static constexpr size_t value = 0;
  // static constexpr size_t value = 1;
  static constexpr size_t value = does_have_ref_base_method_v<IterType>;
};

template <typename BaseIter, typename Policy, size_t DEPTH>
struct get_iter_depth<JumpIterator<BaseIter, Policy, DEPTH>> {
  static constexpr size_t value = DEPTH;
};

template <typename JumpIter>
constexpr size_t get_iter_depth_v = get_iter_depth<JumpIter>::value;


/*template <typename JumpIter, typename = void>
struct rec_jump_iterator {
  using type = JumpIter;
};

template <typename IterType, typename Policy, size_t DEPTH>
class rec_jump_iterator<
  jump_iterator<IterType, Policy, DEPTH>,
  std::void_t<typename jump_iterator<IterType, Policy, DEPTH>::depth_base_type::depth_base_type>
> {
  using DB = typename jump_iterator<IterType, Policy, DEPTH>::depth_base_type;
public:
  using type = jump_iterator<IterType, Policy, DEPTH + DB::NUM_DEPTH>;
};*/

// TODO: Consider implementing rec_jump_iterator<BaseIter, Policy>
// that directly gives you the stacked jump_iterator with the exact
// depth, instead of doing rec_jump_iterator<jump_iterator>, this way
// you may reduce the binary size and compile times, because otherwise, it will generate
// two jump_iterator specialization per rec_jump_iterator call, from
// which you need only one

/*template <typename JumpIter>
struct rec_jump_iterator;

template <typename BaseIter, typename Policy, size_t DEPTH>
class rec_jump_iterator<jump_iterator<BaseIter, Policy, DEPTH>> {
  using DB = jump_iterator<BaseIter, Policy, DEPTH>::depth_base_type;
public:
  using type = jump_iterator<BaseIter, Policy, DEPTH + get_iter_depth_v<DB>>;
};

// Recursive in depth jump iterator
template <typename JumpIter>
using rec_jump_iterator_t = typename rec_jump_iterator<JumpIter>::type;*/

template <typename BaseIter, typename Policy, size_t DEPTH = 1>
class rec_jump_iterator {
  static_assert(DEPTH != 0);
  using DB = get_base_typedef_t<BaseIter, DEPTH - 1>;
public:
  using type = JumpIterator<BaseIter, Policy, DEPTH + get_iter_depth_v<DB>>;
};

// Recursive in depth jump iterator
template <typename BaseIter, typename Policy, size_t DEPTH = 1>
using rec_jump_iterator_t = typename rec_jump_iterator<BaseIter, Policy, DEPTH>::type;


template <typename BaseIter, typename Policy, bool STACKABLE>
struct stack_jump_iterator;

template <typename BaseIter, typename Policy>
struct stack_jump_iterator<BaseIter, Policy, true> {
  using type = JumpIterator<BaseIter, Policy>;
};

template <typename BaseIter, typename Policy>
struct stack_jump_iterator<BaseIter, Policy, false> {
  using type = rec_jump_iterator_t<BaseIter, Policy>;
};

template <typename BaseIter, typename Policy, bool STACKABLE>
using stack_jump_iterator_t = typename stack_jump_iterator<BaseIter, Policy, STACKABLE>::type;



// NOTE:
// Because of the perfect forwarding
//  in make_tensor_iterator/iterable functions
//  we need to ensure that the policy arguments
//  are not const or reference
/*template <typename PolicyArg>
using arg_to_jump_policy_t = 
  _if_t<
    std::is_integral_v<std::remove_cvref_t<PolicyArg>>,
    FixedJumpPolicy<std::remove_cvref_t<PolicyArg>>,
    _if_t<
      is_iterator_v<std::remove_cvref_t<PolicyArg>>,
      JumpByIndexDiff<std::remove_cvref_t<PolicyArg>>,
      std::remove_cvref_t<PolicyArg>
    >
  >;*/

/*template <typename PolicyArg>
using arg_to_jump_policy_t = 
  _if_t<
    std::is_integral_v<PolicyArg>,
    FixedJumpPolicy<PolicyArg>,
    _if_t<
      is_iterator_v<PolicyArg>,
      JumpByIndexDiff<PolicyArg>,
      PolicyArg
    >
  >;*/


// Template Deduction Guide
template <typename IT, typename PolicyArg>
JumpIterator(const IT&, const PolicyArg&)
  -> JumpIterator<
    IT,
    arg_to_jump_policy_t<PolicyArg>
  >;



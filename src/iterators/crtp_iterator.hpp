#pragma once

#include "../meta_func.hpp"

#include <type_traits>
#include <iterator>
#include <tuple>


// TODO: Make GetIterFunc to be be variadic
// then advance all iterators from all GetIterFunc(IterType)


/*template <typename BaseType, typename IterType, typename GetIterFunc = std::identity>
// template <typename NewTypeName, typename IterType>
class CrtpIterator {

  using ThisType = CrtpIterator<BaseType, IterType, GetIterFunc>;

public:

  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  using iterator_category = typename std::iterator_traits<IterType>::iterator_category;
  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;
  
  template <typename ...Args, typename FN = GetIterFunc, typename = std::enable_if_t<
    std::is_constructible_v<IterType, Args...> && std::is_default_constructible_v<FN>>>
  explicit CrtpIterator(Args&&... args)
  : _it(std::forward<Args>(args)...) {}
  
  template <typename IT = IterType, typename = std::enable_if_t<std::is_constructible_v<IterType, IT>>>
  // template <typename IT = IterType, typename = std::enable_if_t<is_iterator_v<IT>>>
  CrtpIterator(const IT& it, const GetIterFunc& get_func)
  : _it(it), _get_it(get_func) {}


  reference operator*() const { return *_get_it(_it); }
  pointer operator->() const { return _get_it(_it).operator->(); }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference operator[](difference_type i) const { return _get_it(_it)[i]; }


  BaseType& operator++() { ++_get_it(_it); return static_cast<BaseType&>(*this); }

  BaseType operator++(int) { BaseType temp(_it); ++_get_it(_it); return temp; }
 

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  BaseType& operator--() { --_get_it(_it); return static_cast<BaseType&>(*this); }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  BaseType operator--(int) { BaseType temp(_it); --_get_it(_it); return temp; }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType& operator+=(difference_type i) { _get_it(_it) += i; return static_cast<BaseType&>(*this); }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType operator+(difference_type i) const { BaseType temp(_it); return temp += i; }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend BaseType operator+(difference_type i, BaseType rhs) { return rhs += i; }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType& operator-=(difference_type i) { _get_it(_it) -= i; return static_cast<BaseType&>(*this); }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType operator-(difference_type i) const { BaseType temp(_it); return temp -= i; }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend difference_type operator-(const BaseType& lhs, const BaseType& rhs) { return lhs._get_it(lhs._it) - rhs._get_it(rhs._it); }


  // bool operator==(const BaseType& other) const { return _it == other._it; }
  // bool operator!=(const BaseType& other) const { return _it != other._it; }

  bool operator==(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) == oth._get_it(oth._it);
  }

  bool operator!=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) != oth._get_it(oth._it);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) < oth._get_it(oth._it);
  }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) <= oth._get_it(oth._it);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) > oth._get_it(oth._it);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return _get_it(_it) >= oth._get_it(oth._it);
  }

// protected:

  using base_type = IterType;
  base_type& base() { return _it; }
  const base_type& base() const { return _it; }

protected:

  IterType _it;
  [[no_unique_address]] GetIterFunc _get_it;

};*/


template <typename BaseType, typename IterObj, typename ...GetIterFuncs>
class CrtpIterator;


template <typename BaseIter, typename IterObj>
class CrtpIterator<BaseIter, IterObj>
: public CrtpIterator<BaseIter, IterObj, std::identity> {
  using BaseType = CrtpIterator<BaseIter, IterObj, std::identity>;
public:
  using BaseType::BaseType;
};


template <typename BaseType, typename IterObj, typename ...GetIterFuncs>
// template <typename NewTypeName, typename IterType>
class CrtpIterator {

  using ThisType = CrtpIterator<BaseType, IterObj, GetIterFuncs...>;

  static constexpr bool FN_DEF_CONSTRUCTABLE
    = (std::is_default_constructible_v<GetIterFuncs> && ...);

  BaseType& self() { return static_cast<BaseType&>(*this); }
  const BaseType& self() const { return static_cast<const BaseType&>(*this); }

public:

  template <size_t ID = 0>
  using IterType = std::remove_cvref_t<decltype(std::declval<nth_type_t<ID, GetIterFuncs...>>()(std::declval<IterObj>()))>;
  
  // using FirstIter = std::remove_cvref_t<decltype(std::declval<first_type_t<GetIterFuncs...>>()(std::declval<IterObj>()))>;
  using FirstIter = IterType<0>;

  // TODO: Calculate most minimal iterator cagerory !
  // using IC = most_base_t<typename

public:

  using difference_type = typename std::iterator_traits<FirstIter>::difference_type;
  using iterator_category = typename std::iterator_traits<FirstIter>::iterator_category;
  using value_type = typename std::iterator_traits<FirstIter>::value_type;
  using reference = typename std::iterator_traits<FirstIter>::reference;
  using pointer = typename std::iterator_traits<FirstIter>::pointer;
  
  template <
    typename ...Args,
    bool COND =
      std::is_constructible_v<IterObj, Args...>
      && FN_DEF_CONSTRUCTABLE,
    typename = std::enable_if_t<COND>
  >
  // template <typename ...Args>
  explicit CrtpIterator(Args&&... args)
  : _it(std::forward<Args>(args)...) {}
  
  template <typename IT = IterObj, typename = std::enable_if_t<
    std::is_constructible_v<IterObj, IT>>>
  CrtpIterator(const IT& it, const std::tuple<GetIterFuncs...>& get_funcs)
  : _it(it), _get_its(get_funcs) {}


  // template <size_t ID>
  // explicit operator IterType<ID>() const { return iter<ID>(); }


  // template <typename N = (GetIterFuncs)>
  reference operator*() const { return *std::get<0>(_get_its)(_it); }
  pointer operator->() const { return std::get<0>(_get_its)(_it).operator->(); }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference operator[](difference_type i) const { return std::get<0>(_get_its)(_it)[i]; }


  BaseType& operator++() {
    // ++_get_its(_it);
    std::apply([&](const GetIterFuncs&... fn){
      (++fn(_it), ...);
    }, _get_its);
    return self();
  }

  BaseType operator++(int) {
    // BaseType temp(_it);
    BaseType temp = self();
    ++(*this);
    return temp;
  }
 

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  BaseType& operator--() {
    // --_get_it(_it);
    std::apply([&](const GetIterFuncs&... fn){
      (--fn(_it), ...);
    }, _get_its);
    return self();
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  BaseType operator--(int) {
    // BaseType temp(_it);
    BaseType temp = self();
    --(*this);
    return temp;
  }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType& operator+=(difference_type i) {
    // _get_it(_it) += i;
    std::apply([&](const GetIterFuncs&... fn){
      ((fn(_it) += i), ...);
    }, _get_its);
    return self();
  }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType operator+(difference_type i) const {
    // BaseType temp(_it);
    BaseType temp = self();
    return temp += i;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend BaseType operator+(difference_type i, BaseType rhs) {
    return rhs += i;
  }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType& operator-=(difference_type i) {
    // _get_it(_it) -= i;
    std::apply([&](const GetIterFuncs&... fn){
      ((fn(_it) -= i), ...);
    }, _get_its);
    return self();
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  BaseType operator-(difference_type i) const {
    // BaseType temp(_it);
    BaseType temp = self();
    return temp -= i;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend difference_type operator-(const BaseType& lhs, const BaseType& rhs) {
    return std::get<0>(lhs._get_its)(lhs._it) - std::get<0>(rhs._get_its)(rhs._it);
  }


  // bool operator==(const BaseType& other) const { return _it == other._it; }
  // bool operator!=(const BaseType& other) const { return _it != other._it; }

  bool operator==(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) == oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) == other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) == std::get<0>(oth._get_its)(oth._it);
  }

  bool operator!=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) != oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) != other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) != std::get<0>(oth._get_its)(oth._it);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) < oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) < other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) < std::get<0>(oth._get_its)(oth._it);
  }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) <= oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) <= other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) <= std::get<0>(oth._get_its)(oth._it);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) > oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) > other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) > std::get<0>(oth._get_its)(oth._it);
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>=(const BaseType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    // return _get_it(_it) >= oth._get_it(oth._it);
    // return std::apply([&](const auto&... get_iters) {
    //   return std::apply([&](const auto&... other_get_iters) {
    //     return ((get_iters(_it) >= other_get_iters(oth._it)) && ...);
    //   }, oth._get_its);
    // }, _get_its);
    return std::get<0>(_get_its)(_it) >= std::get<0>(oth._get_its)(oth._it);
  }

protected:

  using base_type = IterObj;
  base_type& base() { return _it; }
  const base_type& base() const { return _it; }
  
  template <size_t ID = 0> 
  IterType<ID>& iter() { return std::get<ID>(_get_its)(_it); }
  
  template <size_t ID = 0> 
  const IterType<ID>& iter() const { return std::get<ID>(_get_its)(_it); }

protected:

  IterObj _it;
  // [[no_unique_address]] GetIterFunc _get_it;
  [[no_unique_address]] std::tuple<GetIterFuncs...> _get_its;

};


// NOTE: Usage example:
/*template <typename T>
struct test_index_iterator
: public CrtpIterator<test_index_iterator<T>, index_iterator<T>> {
  using ThisType = test_index_iterator<T>;
  using IterType = index_iterator<T>;
  using CrtpType = CrtpIterator<ThisType, IterType>;
  template <typename>
  friend class test_index_iterator;
public:
  using CrtpType::CrtpType;

  template <typename OT>
  test_index_iterator(const test_index_iterator<OT>& other)
  : CrtpType(other.iter()) {}
};*/





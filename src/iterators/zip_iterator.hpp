#pragma once

#include "../meta_func.hpp"
#include "operator_arrow_proxy.hpp"

#include <tuple>
#include <iterator>
#include <type_traits> // enable_if_t
// #include <memory> // unique_ptr

// NOTE: Similar to boost::zip_iterator

// template <typename IndexSeq, typename ...Iters>
// class zip_iterator;

// template <size_t ...Is, typename ...Iters>
// class zip_iterator<std::index_sequence<Is...>, Iters...>
// > {
// };


template <typename ...Iters>
class ZipIterator;


template <size_t ID, typename ...Iters>
inline auto make_unzip_iterator(const ZipIterator<Iters...>& it) {
// inline unzip_iterator<zip_iterator<Iters...>, ID> make_unzip_iterator(const zip_iterator<Iters...>& it) {
  return std::get<ID>(it.tuple());
}


template <typename ...Iters>
class ZipIterator {

  static_assert((is_iterator_v<Iters> && ...));
  using ThisType = ZipIterator<Iters...>;

  using TupType = std::tuple<Iters...>;
  TupType _iters;

public:

  using iterator_category = most_base_t<
    typename std::iterator_traits<Iters>::iterator_category...>;
  // using difference_type =
  //   typename std::iterator_traits<first_type_t<Iters...>>::difference_type;
  using difference_type = std::ptrdiff_t;

  using value_type = std::tuple<typename std::iterator_traits<Iters>::value_type...>;
  using reference = std::tuple<typename std::iterator_traits<Iters>::reference...>;
  // using pointer = std::tuple<typename std::iterator_traits<Iters>::pointer...>;
  // using pointer = std::unique_ptr<reference>;
  using pointer = operator_arrow_proxy<reference>;

  using tuple_type = std::tuple<Iters...>;
  tuple_type& tuple() { return _iters; }
  const tuple_type& tuple() const { return _iters; }

  // using base_type = zip_iterator<typename Iters::base_type...>;
  // auto base() const {
 //    return std::apply([](const Iters&... its){
 //      return zip_iterator(its.base()...);
 //    }, _iters);
  // }
  // auto base() {
 //    return std::apply([](Iters&... its){
 //      return zip_iterator(its.base()...);
 //    }, _iters);
  // }

  template <bool COND = (std::is_default_constructible_v<Iters> && ...),
    typename = std::enable_if_t<COND>>
  ZipIterator() : _iters() {}

  // NOTE: No need for explicit here, because explicit takes action
  //  when the constructor has only one argument, which means that
  //  in the constructor here, we will have a zip_iterator of one iterator
  //  only, which conceptually doesnt change it
  ZipIterator(const Iters&... iters)
  : _iters(iters...) {}
  
  // zip_iterator(const std::tuple<const Iters&...>& iters)
 //  : _iters(iters) {}

  // template <typename ...OIters>
  template <typename ...OIters, typename =
    std::enable_if_t<(std::is_constructible_v<Iters, OIters> && ...)>>
    // std::enable_if_t<(std::is_convertible_v<OIters, Iters> && ...)>>
    // std::enable_if_t<std::is_convertible_v<std::tuple<OIters...>, std::tuple<Iters...>>>>
  ZipIterator(const ZipIterator<OIters...>& other)
  : _iters(other.tuple()) {}
  // : _iters(std::make_from_tuple<TupType>(other.tuple())) {}
  // : _iters(std::apply([](const OIters&... its){
  //  return zip_iterator(Iters(its)...);
  // }, other.tuple())) {}


  // NOTE: Consider this
  // zip_iterator(const std::tuple<Iters...>& tuple_of_iters)
  // // zip_iterator(const std::tuple<const Iters&...>& tuple_of_iters)
  // : _iters(tuple_of_iters) {}
  
  // NOTE: Consider also this
  // template <size_t ID> 
  // explicit operator const std::tuple_element_t<ID, TupType>&() const {
  //  return std::get<ID>(_iters);
  // }


  reference operator*() const {
    return std::apply([](const Iters&... its){
      // return std::make_tuple(*its...);
      return std::forward_as_tuple(*its...);
    }, _iters);
  }

  pointer operator->() const {
    // return std::make_unique(*(*this));
    // return std::make_unique(this->operator*());
    return operator_arrow_proxy(this->operator*());
  }

  template <bool COND = std::is_base_of_v<std::random_access_iterator_tag, iterator_category>
  , typename = std::enable_if_t<COND>>
  reference operator[](difference_type i) const {
    auto temp(*this);
    return *(temp += i);
  }


  ThisType& operator++() {
    std::apply([](Iters&... its){
      (++its, ...);
    }, _iters);
    return *this;
  }

  ThisType operator++(int) {
    ThisType temp(*this);
    ++(*this);
    return temp;
  }
 

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  ThisType& operator--() {
    std::apply([](Iters&... its){
      (--its, ...);
    }, _iters);
    return *this;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  ThisType operator--(int) {
    ThisType temp(*this);
    --(*this);
    return temp;
  }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType& operator+=(difference_type i) {
    std::apply([i](Iters&... its){
      ((its += i), ...);
    }, _iters);
    return *this;
  }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType operator+(difference_type i) const {
    ThisType temp(*this);
    return temp += i;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend ThisType operator+(difference_type i, ThisType rhs) {
    return rhs += i;
  }


  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType& operator-=(difference_type i) {
    std::apply([i](Iters&... its){
      ((its -= i), ...);
    }, _iters);
    return *this;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  ThisType operator-(difference_type i) const {
    ThisType temp(*this);
    return temp -= i;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  friend difference_type operator-(const ThisType& lhs, const ThisType& rhs) {
    return std::get<0>(lhs._iters) - std::get<0>(rhs._iters);
  }

  // =========================================================== //

  // bool operator==(const BaseType& other) const { return _it == other._it; }
  // bool operator!=(const BaseType& other) const { return _it != other._it; }

  // TODO:
  // template <typename ...Iters0, typename ...Iters1>
  // friend bool operator==(const zip_iterator<Iters0...>& lhs, const zip_iterator<Iters1...>& rhs) {
  //  return lhs.it == rhs.it;
  // }

  // BUG:
  // std::vector<int> vec;
  // zip_iterator it(vec.begin());
  // zip_iterator<decltype(vec.cbegin())> cit(it);
  // std::cout << bool(cit == it) << "\n"; // WORKS
  // std::cout << bool(it == cit) << "\n"; // ERROR

  bool operator==(const ThisType& other) const {
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its == oits) && ...);
      }, other._iters);
    }, _iters);
  }

  bool operator!=(const ThisType& other) const {
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its != oits) || ...);
      }, other._iters);
    }, _iters);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<(const ThisType& other) const {
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its < oits) && ...);
      }, other._iters);
    }, _iters);
  }
 
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator<=(const ThisType& other) const {
    const ThisType& oth = static_cast<const ThisType&>(other);
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its <= oits) && ...);
      }, other._iters);
    }, _iters);
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>(const ThisType& other) const {
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its > oits) && ...);
      }, other._iters);
    }, _iters);
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  bool operator>=(const ThisType& other) const {
    return std::apply([&](const Iters&... its){
      return std::apply([&](const Iters&... oits){
        return ((its >= oits) && ...);
      }, other._iters);
    }, _iters);
  }

  // =========================SENTINELS============================ //

  template <typename IT, typename =
    std::enable_if_t<contains_type_v<IT, Iters...>>>
  bool operator==(const IT& other) const {
    return std::get<IT>(_iters) == other;
  }
  
  template <typename IT, typename =
    std::enable_if_t<contains_type_v<IT, Iters...>>>
  friend bool operator==(const IT& lhs, const ThisType& rhs) {
    return lhs == std::get<IT>(rhs._iters);
  }

  template <typename IT, typename =
    std::enable_if_t<contains_type_v<IT, Iters...>>>
  bool operator!=(const IT& other) const {
    return std::get<IT>(_iters) != other;
  }
  
  template <typename IT, typename =
    std::enable_if_t<contains_type_v<IT, Iters...>>>
  friend bool operator!=(const IT& lhs, const ThisType& rhs) {
    return lhs != std::get<IT>(rhs._iters);
  }

  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  bool operator<(const IT& other) const {
    return std::get<IT>(_iters) < other;
  }
  
  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  friend bool operator<(const IT& lhs, const ThisType& rhs) {
    return lhs < std::get<IT>(rhs._iters);
  }
 
  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  bool operator<=(const IT& other) const {
    return std::get<IT>(_iters) <= other;
  }
  
  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  friend bool operator<=(const IT& lhs, const ThisType& rhs) {
    return lhs <= std::get<IT>(rhs._iters);
  }
  
  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  bool operator>(const IT& other) const {
    return std::get<IT>(_iters) > other;
  }
  
  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  friend bool operator>(const IT& lhs, const ThisType& rhs) {
    return lhs > std::get<IT>(rhs._iters);
  }

  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  bool operator>=(const IT& other) const {
    return std::get<IT>(_iters) >= other;
  }

  template <typename IT, typename = std::enable_if_t<
    contains_type_v<IT, Iters...> && 
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<IT>::iterator_category
    >
  >>
  friend bool operator>=(const IT& lhs, const ThisType& rhs) {
    return lhs >= std::get<IT>(rhs._iters);
  }

};


/*template <typename ...Iters>
zip_iterator(const Iters&...)
-> zip_iterator<Iters...>;

template <typename ...Iters>
zip_iterator(const std::tuple<Iters...>&)
// zip_iterator(const std::tuple<const Iters&...>&)
-> zip_iterator<Iters...>;*/


template <typename ...Iters>
ZipIterator<Iters...> make_zip_iterator(const Iters&... iters) {
  return ZipIterator<Iters...>(iters...);
}


// namespace std {


// TODO: Implement it using the CrtpIterator
// , so the type keeps consistent at return of increment operators
template <typename ...Iters>
class std::move_iterator<ZipIterator<Iters...>>
: public ZipIterator<move_iterator<Iters>...> {
  using BaseType = ZipIterator<move_iterator<Iters>...>;
public:

  explicit move_iterator(const ZipIterator<Iters...>& zip_it)
  : BaseType(std::make_from_tuple<BaseType>(
    std::apply([](const Iters&... iters){
      return std::forward_as_tuple(std::make_move_iterator(iters)...);
    }, zip_it.tuple())
  )) {}

  ZipIterator<Iters...> base() const {
    return std::apply([](const move_iterator<Iters>&... iters){
      ZipIterator<Iters...>(iters.base()...);
    }, this->tuple());
  }

  // NOTE: std::move_iterator also has a non-const base() method
  //  but we are unable to give the base type as a reference here
  //  because we have to create it, so we will go without it

};


// NOTE: We will just rely on the specialization above
// template <typename ...Iters>
// zip_iterator<move_iterator<Iters...>>
// make_move_iterator(const zip_iterator<Iters...>& it) {
//  return std::apply([](const Iters&... iters){
//    return make_zip_iterator(make_move_iterator(iters)...);
//  }, it.tuple());
// }


// };



#pragma once

// #include "../meta_func.hpp"
#include "operator_arrow_proxy.hpp"
// #include "view.hpp" // for the transformed view implementation

#include <iterator>
#include <type_traits> // invoke_result_t
// #include <memory> // unique_ptr
// #include <functional>

// TODO: Make it work with unary functions that have multiple
// overloads. Make it to choose the overload of the function
// that uses the base iterator value type as argument

// TODO: Make it with multiple number of base iterators and the Functor
// to be with number of arguments that matches the number of base iters

// Similar to boost::transform_iterator
// https://www.boost.org/doc/libs/1_74_0/libs/iterator/doc/transform_iterator.html
// https://www.boost.org/doc/libs/1_88_0/boost/intrusive/detail/transform_iterator.hpp


/*template <typename T>
struct pointer_to_function { using type = T; };

template <typename R, typename... Args>
struct pointer_to_function<R(*)(Args...)> {
    using type = R(Args...);
};

template <typename T>
using pointer_to_function_t = typename pointer_to_function<T>::type;*/

// template <typename T>
// using pointer_to_function_t = decltype(+std::declval<T>());

template <typename IterType, typename UnaryFunc>
class TransformIterator;


template <typename Iter, typename Func>
TransformIterator(const Iter&, const Func&)
-> TransformIterator<Iter, Func>;


// template <typename ContType, typename UnaryFunc>
// [[nodiscard]] inline auto transformed(ContType&& cont, UnaryFunc&& func) {
//   static_assert(is_range_v<std::remove_reference_t<ContType>>);
//   return View(
//     TransformIterator(std::forward<ContType>(cont).begin(), std::forward<UnaryFunc>(func)),
//     TransformIterator(std::forward<ContType>(cont).end(), std::forward<UnaryFunc>(func))
//   );
// }


template <
  typename IterType,
  typename UnaryFunc
  // typename UnaryFunc = std::function<typename IterType::reference>
>
class TransformIterator {
public:
  
  // static_assert(is_iterator_v<IterType>);
  // BUG: until is_unary_function gives correct results for unary functor classes,
  //      this will stay commented out
  // static_assert(is_unary_function_v<UnaryFunc>);

  using base_type = IterType;

  // If the given UnaryFunc is a function type,
  //  it will then convert it to a function pointer type
  //  so we are able to store it as a member variable
  // using functor_type = UnaryFunc;
  // using functor_type = _if_t<
  //   std::is_function_v<UnaryFunc>,
  //   function_to_pointer_t<UnaryFunc>,
  //   UnaryFunc
  // >;
  
  // using functor_type = function_to_pointer_t<UnaryFunc>;
  using functor_type = std::decay_t<UnaryFunc>;

private:

  using ThisType = TransformIterator<IterType, UnaryFunc>;

  base_type _it;
  [[no_unique_address]] functor_type _func;

public:

  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  using iterator_category = typename std::iterator_traits<IterType>::iterator_category;

  // using reference = decltype(std::declval<functor_type>()(*std::declval<IterType>()));
  using reference = std::invoke_result_t<functor_type, typename std::iterator_traits<IterType>::reference>;
  using value_type = std::remove_cvref_t<reference>;

  static_assert(!std::is_same_v<void, value_type>,
    "The Unary Function should return a NON-void type");

  using pointer = std::conditional_t<
    std::is_reference_v<reference>,
    std::remove_reference_t<reference>*,
    // std::unique_ptr<value_type>
    operator_arrow_proxy<reference>
  >;


  template <typename ...Args, typename UF = UnaryFunc, typename = std::enable_if_t<
    std::is_constructible_v<IterType, Args...> && std::is_default_constructible_v<UF>>>
  explicit TransformIterator(Args&&... args)
    : _it(std::forward<Args>(args)...), _func() {}


  // template <typename FuncArg = UnaryFunc, typename = std::enable_if_t<
  //   std::is_constructible_v<UnaryFunc, FuncArg>>>
  // TransformIterator(const IterType& it, FuncArg&& func)
  //   : _it(it), _func(std::forward<FuncArg>(func)) {}
  
  TransformIterator(const IterType& it, const UnaryFunc& func)
    : _it(it), _func(func) {}


  template <typename IT, typename UF, typename = std::enable_if_t<
    std::is_constructible_v<IterType, IT> && std::is_constructible_v<UnaryFunc, UF>>>
  TransformIterator(const TransformIterator<IT, UF>& other)
    : _it(other.base()), _func(other.functor()) {}
  
  // template <typename IT, typename = std::enable_if_t<
  //   std::is_convertible_v<IT, IterType>>>
  //   std::is_constructible<IterType, IT>>>
  // TransformIterator(const TransformIterator<IT, UnaryFunc>& other)
  //   : _it(other.base()), _func(other.functor()) {}


  // NOTE: Allowing this, we dont need the comparison operators overloaded here
  //        Or do we ? (Vsause Theme song ...)
  // operator const IterType&() const { return _it; }

  // NOTE: Consider This given the base iterator, it only assigns it, without changing the functor
  // ThisType& operator=(const IterType& iter) { _it = iter; return *this; }

  // ========================================================= //


  const base_type& base() const { return _it; }

  // NOTE: Needed for some tricks with jump_iterator (to make it possible
  // for the jump_iterator's policy to act on the underlying iterator)
  base_type& base() { return _it; }

  const functor_type& functor() const { return _func; }
  // functor_type& functor() { return _func; }


  reference operator*() const {
    return _func(*_it);
  }
 
  pointer operator->() const {
    if constexpr (std::is_reference_v<reference>)
      return &_func(*_it);
    else
      // return std::make_unique(_func(*_it));
      return operator_arrow_proxy(_func(*_it));
  }
  
  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  reference operator[](difference_type i) const {
    auto temp(*this);
    return *(temp += i);
  }

  // ================================================================= //

  ThisType& operator++() {
    ++_it;
    return *this;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::bidirectional_iterator_tag, IC>>>
  ThisType& operator--() {
    --_it;
    return *this;
  }

  ThisType operator++(int) {
    auto temp(*this);
    ++(*this);
    return temp;
  }
 
  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::bidirectional_iterator_tag, IC>>>
  ThisType operator--(int) {
    auto temp(*this);
    --(*this);
    return temp;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  ThisType& operator+=(difference_type i) {
    _it += i;
    return *this;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  ThisType& operator-=(difference_type i) {
    _it -= i;
    return *this;
  }


  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  ThisType operator+(difference_type i) const {
    auto temp(*this);
    return temp += i;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  ThisType operator-(difference_type i) const {
    auto temp(*this);
    return temp -= i;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  difference_type operator-(const ThisType& rhs) const {
    return _it - rhs._it;
  }

  // =============================================================== //

  // NOTE: We dont need these comparisons, if we have this above:
  //  operator const IterType&() const { return _it; }

  bool operator==(const ThisType& other) const {
    return _it == other._it; // but we can only take the difference between the id iterators
  }

  bool operator!=(const ThisType& other) const {
    return _it != other._it;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator<(const ThisType& other) { return _it < other._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator<=(const ThisType& other) { return _it <= other._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator>(const ThisType& other) { return _it > other._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator>=(const ThisType& other) { return _it >= other._it; }

  // =========================SENTINELS============================ //

  bool operator==(const IterType& other) const {
    return _it == other;
  }

  friend bool operator==(const IterType& lhs, const ThisType& rhs) {
    return lhs == rhs._it;
  }

  bool operator!=(const IterType& other) const {
    return _it != other;
  }

  friend bool operator!=(const IterType& lhs, const ThisType& rhs) {
    return lhs != rhs._it;
  }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator<(const IterType& other) { return _it < other; }
  
  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  friend bool operator<(const IterType& lhs, const ThisType& rhs) { return lhs < rhs._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator<=(const IterType& other) { return _it <= other; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  friend bool operator<=(const IterType& lhs, const ThisType& rhs) { return lhs <= rhs._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator>(const IterType& other) { return _it > other; }
  
  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  friend bool operator>(const IterType& lhs, const ThisType& rhs) { return lhs > rhs._it; }

  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  bool operator>=(const IterType& other) { return _it >= other; }
  
  template <
    typename IC = iterator_category, 
    typename = std::enable_if_t<std::is_base_of_v<
      std::random_access_iterator_tag, IC>>>
  friend bool operator>=(const IterType& lhs, const ThisType& rhs) { return lhs >= rhs._it; }

};



// ============================= OPTIMIZATION ================================= //

/*
// BUG: The class template argument deduction acts weird:
// NestUnaryFunc nfunc(fFunc0(), fFunc1()); // Use of class template 'NestUnaryFunc' requires template arguments; argument deduction not allowed in function return type
// auto nfunc = NestUnaryFunc(fFunc0(), fFunc1()); // Works

template <typename UnaryFunc0, typename UnaryFunc1>
class NestUnaryFunc {
public:

  using unary_func0 = function_to_pointer_t<UnaryFunc0>;
  using unary_func1 = function_to_pointer_t<UnaryFunc1>;

  // NestUnaryFunc(const unary_func0& func0, const unary_func1& func1)
  NestUnaryFunc(const UnaryFunc0& func0, const UnaryFunc1& func1)
  : _func0(func0), _func1(func1) {}

  template <typename Val>
  decltype(auto) operator()(Val&& val) const {
    return _func1(_func0(std::forward<Val>(val)));
  }

  const unary_func0& func0() const { return _func0; }
  const unary_func1& func1() const { return _func1; }
  
private:

  [[no_unique_address]] unary_func0 _func0;
  [[no_unique_address]] unary_func1 _func1;

};

// template <typename Func0, typename Func1>
// NestUnaryFunc(const Func0&, const Func1&)
// // -> NestUnaryFunc<pointer_to_function_t<Func0>, pointer_to_function_t<Func1>>;
// -> NestUnaryFunc<Func0, Func1>;


template <typename Iter, typename Func0, typename Func1>
class TransformIterator<TransformIterator<Iter, Func0>, Func1>
: public CrtpIterator<
    TransformIterator<TransformIterator<Iter, Func0>, Func1>,
    TransformIterator<Iter, NestUnaryFunc<Func0, Func1>>
  > {

  using ThisType = TransformIterator<TransformIterator<Iter, Func0>, Func1>;
  using IterType = TransformIterator<Iter, NestUnaryFunc<Func0, Func1>>;
  using CrtpType = CrtpIterator<ThisType, IterType>;

public:

  TransformIterator(const TransformIterator<Iter, Func0>& it, const Func1& func)
  : CrtpType(it.base(), NestUnaryFunc(it.functor(), func)) {}

  template <typename IT, typename FN0, typename FN1, typename = std::enable_if_t<
    std::is_constructible_v<typename IterType::base_type, IT> && std::is_constructible_v<Func0, FN0> && std::is_constructible_v<Func1, FN1>>>
  TransformIterator(const TransformIterator<TransformIterator<IT, FN0>, FN1>& other)
  : CrtpType(other.base().base(), NestUnaryFunc(other.base().functor(), other.functor())) {}

  // TransformIterator(const IterType& other)
  // : CrtpType(other) {}
  
  using CrtpType::CrtpType;

  // template <typename IT, typename FN>
  // TransformIterator(const TransformIterator<IT, FN>& other)
  // : CrtpType(other.base(), other.func()) {}

  using base_type = TransformIterator<Iter, Func0>;
  using functor_type = Func1;

  base_type base() const { return base_type(CrtpType::iter().base(), CrtpType::iter().functor().func0()); }
  const functor_type& functor() const { return CrtpType::iter().functor().func1(); }

};
*/



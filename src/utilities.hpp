#pragma once

// #include "iterators/iterable.hpp"
// #include "iterators/view.hpp"
#include "meta_func.hpp"

#include "glm/detail/qualifier.hpp"
// #include <glm/common.hpp> // glm::vec

#include <type_traits> // std::is_base_of_v
#include <chrono>
#include <iostream>
#include <ostream>    // operator<< overloading for glm::vec
#include <utility> // index_sequence, swap
#include <algorithm>  // for_each

#ifndef uint
typedef unsigned int uint;
#endif


// https://www.youtube.com/watch?v=YG4jexlSAjc
class Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

public:
  Timer() {
    start_time = std::chrono::high_resolution_clock::now();
  }

  void stop() {
    auto end_time = std::chrono::high_resolution_clock::now();

    auto start_ms = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
    auto end_ms = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();

    double ms = double(end_ms - start_ms) * 0.001;

    std::cout << ms << " ms\n";
  }

  ~Timer() {
    stop();
  }
};


template <glm::length_t L, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& stream, const glm::vec<L, T, Q>& v) {
  stream << "{";
  for(int i = 0; i < L; ++i) {
    stream << v[i];
    if (i + 1 != L)
      stream << ", ";
  }

  stream << "}";

  return stream;
}


//struct base_handle {};

template <typename AdvanceIter, typename CompareIter>
inline void advance(AdvanceIter& it, CompareIter first, CompareIter last) {
  constexpr bool is_advance_iter_random_access = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<AdvanceIter>::iterator_category>;
  constexpr bool is_compare_iter_random_access = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<CompareIter>::iterator_category>;

  if constexpr (
    is_advance_iter_random_access && 
    is_compare_iter_random_access
  ) {
    it += last - first;
  }
  else {
    for(; first != last; ++first)
      ++it;
  }
}


template <typename AdvanceIter, typename CompareIter>
inline void radvance(AdvanceIter& it, CompareIter first, CompareIter last) {
  constexpr bool is_advance_iter_random_access = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<AdvanceIter>::iterator_category>;
  constexpr bool is_compare_iter_random_access = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<CompareIter>::iterator_category>;

  if constexpr (
    is_advance_iter_random_access && 
    is_compare_iter_random_access
  ) {
    it -= last - first;
  }
  else {
    for(; first != last; ++first)
      --it;
  }
}


struct HashIterToPtr {

  template <typename IterType>
  int operator()(const IterType& it) const {
    using pointer = typename std::iterator_traits<IterType>::pointer;
    using reference = typename std::iterator_traits<IterType>::reference;
    static_assert(is_iterator_v<IterType>);
    static_assert(std::is_same_v<std::remove_reference_t<reference>*, pointer>);

    // return std::hash<pointer>()(it.operator->());
    return std::hash<pointer>()(&(*it));
  }

};


struct LessIterToPtr {
  template <typename IterType>
  int operator()(const IterType& lhs, const IterType& rhs) const {
    using pointer = typename std::iterator_traits<IterType>::pointer;
    using reference = typename std::iterator_traits<IterType>::reference;
    static_assert(is_iterator_v<IterType>);
    static_assert(std::is_same_v<std::remove_reference_t<reference>*, pointer>);

    return std::less<pointer>()(&(*lhs), &(*rhs));
  }
};



/*template <typename... Types>
class TypePack {
  typedef Types types;
}
*/

 
template <typename PairType>
struct PairToFirst {
  const typename PairType::first_type&
  operator()(const PairType& kvp) const {
    return kvp.first;
  }
};

template <typename PairType>
struct PairToSecond {
  const typename PairType::second_type&
  operator()(const PairType& kvp) const {
    return kvp.second;
  }
};


struct IterToPtr {
  template <typename IterType>
  typename std::iterator_traits<IterType>::pointer
  operator()(const IterType& it) {
    // return &(*it);
    return it.operator->();
  }
};


// --------------------------------------------------------------------------------


// https://www.fluentcpp.com/2021/03/05/stdindex_sequence-and-its-improvement-in-c20/
// NOTE: Equivalent to std::apply([&](auto&&... args) { (f(args), ...); }, tup);
/*template <class F, class Tuple, typename = std::enable_if_t<is_boost_tuple_v<Tuple> || is_std_tuple_v<Tuple>>>
constexpr decltype(auto) for_each(F&& f, Tuple&& tup) {
  if constexpr (is_std_tuple_v<Tuple>) {
    return [] <std::size_t... I>
    (Tuple&& tup, F&& f, std::index_sequence<I...>)
    {
      (f(std::get<I>(tup)), ...);
      return f;
    }(std::forward<Tuple>(tup), std::forward<F>(f),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
  }
  else if constexpr (is_boost_tuple_v<Tuple>) {
    return [] <std::size_t... I>
    (Tuple&& tup, F&& f, std::index_sequence<I...>)
    {
      (f(boost::get<I>(tup)), ...);
      return f;
    }(std::forward<Tuple>(tup), std::forward<F>(f),
      std::make_index_sequence<boost::tuples::length<std::remove_reference_t<Tuple>>::value>{});
  }
}


// For Each Element
// NOTE: No need for that function, just use fold expression,
//  as in the implementation here
// template <typename F, typename ...Args>
// constexpr void for_each(F&& f, Args&&... args) {
// // constexpr decltype(auto) for_each_el(F&& f, Args&&... args) {
//   (f(std::forward<Args>(args)), ...);
//   // return f;
// }


// https://chatgpt.com/c/67d13943-c810-800c-bf3f-2202904f2003
template <class Tuple, class UnaryFunc, typename = std::enable_if_t<is_boost_tuple_v<Tuple> || is_std_tuple_v<Tuple>>>
constexpr auto transform(Tuple&& tup, UnaryFunc&& f) {
  if constexpr (is_std_tuple_v<Tuple>) {
    return []<std::size_t... I>(Tuple&& t, UnaryFunc&& func, std::index_sequence<I...>) {
      return std::make_tuple(func(std::get<I>(t))...);
    }(std::forward<Tuple>(tup), std::forward<UnaryFunc>(f),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
  }
  else if constexpr (is_boost_tuple_v<Tuple>) {
    return []<std::size_t... I>(Tuple&& t, UnaryFunc&& func, std::index_sequence<I...>) {
      return boost::make_tuple(func(boost::get<I>(t))...);
    }(std::forward<Tuple>(tup), std::forward<UnaryFunc>(f),
      std::make_index_sequence<boost::tuples::length<std::remove_reference_t<Tuple>>::value>{});
  }
}

template <typename OutputTuple, typename ...Args>
constexpr OutputTuple assign_tuple(Args&&...) {

}

template <typename InputTuple, typename OutputTuple>
constexpr OutputTuple convert_tuple(const InputTuple& in_tup) {

}*/

// NOTE: auto and decltype(auto) as a function return type
// differ in that the auto will remove cv qualifier and references
// but decltype(auto) will preserve the type as is
// so it if you return std::forward<T>, then define the function as decltype(auto)

template <typename T0, typename ...T1toN>
constexpr decltype(auto) get_first_val(T0&& first, T1toN&&... rest) {
  return std::forward<T0>(first);
}


/*template <typename T, typename Tup>
constexpr decltype(auto) get_any(Tup&& tup) {
  if constexpr (contains_type_v<Tup, T>)
    return std::get<T>(std::forward<Tup>(tup));
  if constexpr (contains_type_v<Tup, T&>)
    return std::get<T&>(std::forward<Tup>(tup));
  if constexpr (contains_type_v<Tup, const T>)
    return std::get<const T>(std::forward<Tup>(tup));
  if constexpr (contains_type_v<Tup, const T&>)
    return std::get<const T&>(std::forward<Tup>(tup));
}*/

template <typename T, typename Tup>
constexpr decltype(auto) get_any(const Tup& tup) {
  if constexpr (contains_type_v<Tup, T>)
    return std::get<T>(tup);
  if constexpr (contains_type_v<Tup, T&>)
    return std::get<T&>(tup);
  if constexpr (contains_type_v<Tup, const T>)
    return std::get<const T>(tup);
  if constexpr (contains_type_v<Tup, const T&>)
    return std::get<const T&>(tup);
}

template <typename T, typename Tup>
constexpr decltype(auto) get_any(Tup& tup) {
  if constexpr (contains_type_v<Tup, T>)
    return std::get<T>(tup);
  if constexpr (contains_type_v<Tup, T&>)
    return std::get<T&>(tup);
  if constexpr (contains_type_v<Tup, const T>)
    return std::get<const T>(tup);
  if constexpr (contains_type_v<Tup, const T&>)
    return std::get<const T&>(tup);
}


// using std::swap;

namespace std {


template <typename ...Ts, typename =
  std::enable_if_t<(!std::is_const_v<Ts> && ...)>>
void swap(std::tuple<Ts&...> lhs, std::tuple<Ts&...> rhs) {
  // static_assert((!std::is_const_v<Ts> && ...));
  std::apply([&](Ts&... lvals){
    std::apply([&](Ts&... rvals){
      (std::swap(lvals, rvals), ...);
    }, rhs);
  }, lhs);
}


}; // namespace std


struct EmptyType {};



// =========================================================== //


// NOTE: Similar to std::shift_right, but with std::rotate
template <typename IterType>
inline constexpr IterType chain_right(IterType first, IterType last, typename std::iterator_traits<IterType>::difference_type n = 1) {
  static_assert(!is_const_iterator_v<IterType>);

  if (n < 0)
    throw std::out_of_range("Cannot chain_right with a negative amount");
  
  using value_type = typename std::iterator_traits<IterType>::value_type;

  while(n--)
    if constexpr (std::is_move_assignable_v<value_type>)
      *(last++) = std::move(*(first++));
    else
      *(last++) = *(first++);

  return first;
}


// NOTE: Similar to std::shift_left, but with std::rotate
template <typename IterType>
inline constexpr IterType chain_left(IterType first, IterType last, typename std::iterator_traits<IterType>::difference_type n = 1) {
  // return chain_right(std::make_reverse_iterator(last), std::make_reverse_iterator(first), n);
  static_assert(!is_const_iterator_v<IterType>);
  
  if (n < 0)
    throw std::out_of_range("Cannot chain_right with a negative amount");

  using value_type = typename std::iterator_traits<IterType>::value_type;

  while(n--)
    if constexpr (std::is_move_assignable_v<value_type>)
      *(--first) = std::move(*(--last));
    else
      *(--first) = *(--last);

  return first;
}


template <typename ...Ts>
inline std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tup) {
  // std::apply([&os](const Ts&... vals){
  //   // (os << vals, ...);
  //   (os << ... << vals);
  // }, tup);

  os << "(";
  std::apply([&os](const Ts&... vals) {
    std::size_t i = 0;
    ((os << (i++ ? ", " : "") << vals), ...);
  }, tup);
  os << ")";

  return os;
}

template <typename Range, typename = std::enable_if_t<is_range_v<Range>>>
inline std::ostream& operator<<(std::ostream& stream, const Range& range) {
  stream << "{";
  for(auto it = range.begin(); it != range.end();) {
    stream << *it;
    if (++it != range.end())
      stream << ", ";
  }
  stream << "}";

  return stream;
}


template <typename Cont>
void print_cont(const Cont& cont) {
  static_assert(is_range_v<Cont>);
  for(const auto& x : cont)
    std::cout << x << "\n";
}

template <typename IterType>
void print_iter(IterType it, long long n) {
  static_assert(is_iterator_v<IterType>);
  while(n--) std::cout << *(it++) << "\n";
}

template <typename IterType>
void print_iter(IterType first, IterType last) {
  static_assert(is_iterator_v<IterType>);
  while(first++ != last)
    std::cout << *first << "\n";
}


// --------------------------------------------------------------------------- //

// NOTE: Recursive for each, that given any nested type of tuples, and ranges
//  it will apply the unary function for each encountered ValueType
template <typename ApplyValue, typename UnaryFunc, typename Range>
inline void for_each_rec(Range&& range, UnaryFunc&& func) {
  using RG = std::remove_reference_t<Range>;
  if constexpr (
    // std::is_convertible_v<Range, ApplyValue>
    std::is_same_v<Range, ApplyValue>
    // || std::is_same_v<const Range, ApplyValue>
    // std::is_same_v<std::remove_cvref_t<Range>, std::remove_cvref_t<ApplyValue>>
  ) {
    func(std::forward<Range>(range));
  }
  else if constexpr (is_range_v<RG>) {
    // std::ranges::for_each(std::forward<Range>(range), [&func](typename RG::reference x) {
    // std::for_each(
    //   std::forward<Range>(range).begin(),
    //   std::forward<Range>(range).end(),
    //   [&func](typename RG::reference x
    // ) {
    //   for_each_rec<ValueType>(x, func);
    // });

    for (decltype(auto) x : std::forward<Range>(range))
    // for (typename RG::reference x : std::forward<Range>(range))
      for_each_rec<ApplyValue>(x, std::forward<UnaryFunc>(func));
  }
  else if constexpr (is_std_tuple_v<RG>) {
    std::apply([&func]<typename ...R>(R&&... vals) {
      (for_each_rec<ApplyValue>(std::forward<R>(vals), std::forward<UnaryFunc>(func)), ...);
    }, std::forward<Range>(range));
  }
}

// TEST:
/*ZipIterator it(IndexIterator(0), IndexIterator('A'), IndexIterator(0));
std::vector<std::tuple<int, char, const int>> vec(it, it + 12);
Matrix<decltype(vec)> mat(vec, 3);

for_each_rec<int&>(mat, [](int& x){
// for_each_rec<int&>(static_cast<const decltype(mat)&>(mat), [](int& x){
  x += 69;
});

for_each_rec<const int&>(mat, [](const int& x){
  std::cout << x << ", ";
});
std::cout << "\n";

print_cont(mat);

return 0;*/


// TODO: Implement for_each function that is generalized
// to work both with iterators and circulators !!!




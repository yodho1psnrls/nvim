#pragma once

#include "../meta_func.hpp" // most_base_t
// #include "view.hpp"
#include <ranges>

#include <iterator>
#include <type_traits>
#include <ranges> // default_sentinel_t

// TODO: Make the last template argument to be a boolean
// which will decide if the FilterIterator adaptor is
// forward or bidirectional, because if its bidirectional
// it needs to store an additional iterator for the front
// , not only for the end

// NOTE: Iterator adaptor that filters a range or iterators.
// You should give it the current IterType, the end IterType
// (so it doesnt overshoot if the last element happened to be filtered out)
// and a FilterType which can be either a unary function from the IterType's value to bool
// or an iterator with a value of bool
// (so if you have a boolean vector bitmask of marked elements, it can filter them out)
template <typename IterType, typename FilterType, typename Sentinel = IterType>
// template <typename IterType, typename FilterType, bool EQUAL = true>
class FilterIterator;

template <typename IterType, typename FilterType>
inline FilterIterator<IterType, FilterType> make_filter_iterator
(const IterType& it, const FilterType& filter) {
  return FilterIterator<IterType, FilterType>(it, filter);
}

template <typename IterType, typename Sentinel, typename FilterType>
inline FilterIterator<IterType, FilterType, Sentinel> make_filter_iterator
(const IterType& it, const Sentinel& end, const FilterType& filter) {
  return FilterIterator<IterType, FilterType, Sentinel>(it, end, filter);
}


template <typename Cont, typename FilterFunc>
inline auto filtered(Cont&& cont, FilterFunc&& filter) {
  static_assert(is_range_v<std::remove_reference_t<Cont>>);

  // if constexpr (is_iterator_v<std::remove_reference_t<FilterFunc>>)
  //  return View(
  //    FilterIterator(std::forward<Cont>(cont).begin(), std::forward<Cont>(cont).end(), std::forward<FilterFunc>(filter)),
  //    // FilterIterator(std::forward<Cont>(cont).begin(), std::forward<FilterFunc>(filter)),
  //    FilterIterator(std::forward<Cont>(cont).end(), std::next(std::forward<FilterFunc>(filter), std::forward<Cont>(cont).size())),
  //  );
  // else
    // return View(
    //   make_filter_iterator<EQUAL>(std::forward<Cont>(cont).begin(), std::forward<Cont>(cont).end(), std::forward<FilterFunc>(filter)),
    //   // make_filter_iterator<INVERSED>(std::forward<Cont>(cont).begin(), std::forward<FilterFunc>(filter)),
    //   make_filter_iterator<EQUAL>(std::forward<Cont>(cont).end(), std::forward<Cont>(cont).end(), std::forward<FilterFunc>(filter))
    // );
  
  return std::ranges::subrange(
    make_filter_iterator(
      std::forward<Cont>(cont).begin(),
      std::forward<Cont>(cont).end(),
      std::forward<FilterFunc>(filter)
    ),
    std::default_sentinel_t()
  );
}

// NOTE: FilterIterator adaptor, with FilterType being either a
// unary function that returns bool or an iterator with value of bool


template <typename IterType, typename FilterType, typename Sentinel>
class FilterIterator {
public:

  static_assert(is_iterator_v<IterType>);
  // static_assert(std::sentinel_for<Sentinel, IterType>);

  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  using iterator_category = std::forward_iterator_tag;
  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;
  
  using base_type = IterType;
  using filter_type = function_to_pointer_t<FilterType>;
  using sentinel_type = Sentinel;

  [[nodiscard]] const base_type& base() const { return _it; }
  [[nodiscard]] const filter_type& filter() const { return _pred; }
  [[nodiscard]] const sentinel_type& sentinel() const { return _end; }

  // TODO: Assert this without breaking
  // static_assert(is_iterator_v<FilterType> || std::is_convertible_v<std::invoke_result_t<FilterType, reference>, bool>);

public:

  template <bool COND = std::is_default_constructible_v<FilterType>,
    typename = std::enable_if_t<COND>>
  explicit FilterIterator(const IterType& iter)
  : _it(iter), _end(iter), _pred() {}
  
  template <bool COND = std::is_default_constructible_v<FilterType>,
    typename = std::enable_if_t<COND>>
  explicit FilterIterator(const IterType& iter, const Sentinel& end)
  : _it(iter), _end(end), _pred() {
    if constexpr (is_iterator_v<FilterType>)
      // while(_it != _end && !*_pred == EQUAL) { ++_it; ++_pred; }
      while(_it != _end && !*_pred) { ++_it; ++_pred; }
    else
      // while(_it != _end && !_pred(*_it) == EQUAL) ++_it;
      while(_it != _end && !_pred(*_it)) ++_it;
  }

  FilterIterator(const IterType& iter, const FilterType& pred)
  : _it(iter), _end(iter), _pred(pred) {}
  
  FilterIterator(const IterType& iter, const Sentinel& end, const FilterType& pred)
  : _it(iter), _end(end), _pred(pred) {
    if constexpr (is_iterator_v<FilterType>)
      // while(_it != _end && !*_pred == EQUAL) { ++_it; ++_pred; }
      while(_it != _end && !*_pred) { ++_it; ++_pred; }
    else
      // while(_it != _end && !_pred(*_it) == EQUAL) ++_it;
      while(_it != _end && !_pred(*_it)) ++_it;
  }
  
  template <typename IT, typename S, typename = std::enable_if_t<std::is_constructible_v<IterType, IT>>>
  FilterIterator(const FilterIterator<IT, FilterType, S>& other)
  : _it(other.base()), _end(other.sentinel()), _pred(other.filter()) {
    // NOTE: We assume that it is in a valid state, since it comes
    // from another FilterIterator with the same predicate
    // while(!_pred(*_it)) ++_it;
  }

  reference operator*() const {
    return _it.operator*();
  }
  
  pointer operator->() const {
    return _it.operator->();
  }

  bool operator==(const FilterIterator& other) const {
    if (_end != other._end)
      throw std::invalid_argument("Cannot compare two iterators with different end");
    return _it == other._it;
  }

  bool operator!=(const FilterIterator& other) const {
    if (_end != other._end)
      throw std::invalid_argument("Cannot compare two iterators with different end");
    return _it != other._it;
  }


  bool operator==(std::default_sentinel_t) const {
    return _it == _end;
  }

  bool operator!=(std::default_sentinel_t) const {
    return _it != _end;
  }

  friend bool operator==(std::default_sentinel_t, const FilterIterator& rhs) {
    return rhs._it == rhs._end;
  }

  friend bool operator!=(std::default_sentinel_t, const FilterIterator& rhs) {
    return rhs._it != rhs._end;
  }


  FilterIterator& operator++() {
    if constexpr (is_iterator_v<FilterType>)
      do {
        ++_it;
        ++_pred;
      // } while(_it != _end && !*_pred == EQUAL);
      } while(_it != _end && !*_pred);
    else
      do {
        ++_it;
      // } while(_it != _end && !_pred(*_it) == EQUAL);
      } while(_it != _end && !_pred(*_it));

    return *this;
  }

  FilterIterator operator++(int) {
    FilterIterator temp(*this);
    ++(*this);
    return temp;
  }

private:

  base_type _it;
  [[no_unique_address]] sentinel_type _end; // NOTE: hard stop, so it doesnt overshoout out of bounds !!!
  [[no_unique_address]] filter_type _pred;

};


/*template <typename IterType, typename FilterType>
class FilterIterator {
public:

  using difference_type = typename std::iterator_traits<IterType>::difference_type;

  using iterator_category = most_base_t<
    std::bidirectional_iterator_tag,
    typename std::iterator_traits<IterType>::iterator_category
    // TODO: There should also be the category of the FilterType, if it is an iterator
  >;

  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;
  
  using base_type = IterType;
  using filter_type = function_to_pointer_t<FilterType>;

  [[nodiscard]] const base_type& base() const { return _it; }
  [[nodiscard]] const filter_type& filter() const { return _pred; }

  // TODO: Assert this without breaking
  // static_assert(is_iterator_v<FilterType> || std::is_convertible_v<std::invoke_result_t<FilterType, reference>, bool>);

public:

  template <bool COND = std::is_default_constructible_v<FilterType>,
    typename = std::enable_if_t<COND>>
  explicit FilterIterator(const IterType& iter)
  : _it(iter), _pred() {
    if constexpr (is_iterator_v<FilterType>)
      while(!*_pred) { ++_it; ++_pred; }
    else
      while(!_pred(*_it)) ++_it;
  }

  FilterIterator(const IterType& iter, const FilterType& pred)
  : _it(iter), _pred(pred) {
    if constexpr (is_iterator_v<FilterType>)
      while(!*_pred) { ++_it; ++_pred; }
    else
      while(!_pred(*_it)) ++_it;
  }
  
  template <typename IT, typename = std::enable_if_t<std::is_constructible_v<IterType, IT>>>
  FilterIterator(const FilterIterator<IT, FilterType>& other)
  : _it(other.base()), _pred(other.filter()) {
    // NOTE: We assume that it is in a valid state, since it comes
    // from another FilterIterator with the same predicate
    // while(!_pred(*_it)) ++_it;
  }

  reference operator*() const {
    return _it.operator*();
  }
  
  pointer operator->() const {
    return _it.operator->();
  }

  bool operator==(const FilterIterator& other) const {
    return _it == other._it;
  }
  
  bool operator!=(const FilterIterator& other) const {
    return _it != other._it;
  }

  FilterIterator& operator++() {
    if constexpr (is_iterator_v<FilterType>)
      do {
        ++_it;
        ++_pred;
      } while(!*_pred);
    else
      do {
        ++_it;
      } while(!_pred(*_it));

    return *this;
  }

  FilterIterator operator++(int) {
    FilterIterator temp(*this);
    ++(*this);
    return temp;
  }
  
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  FilterIterator& operator--() {
    if constexpr (is_iterator_v<FilterType>)
      do {
        --_it;
        --_pred;
      } while(!*_pred);
    else
      do {
        --_it;
      } while(!_pred(*_it));
    return *this;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  FilterIterator operator--(int) {
    FilterIterator temp(*this);
    --(*this);
    return temp;
  }

private:

  base_type _it;
  [[no_unique_address]] filter_type _pred;

};*/


// template <typename IterType, typename FilterIter>
// class FilterIterator<
//  IterType,
//  FilterIter,
//  // std::void_t<typename std::iterator_traits<FilterIter>::iterator_category>
//  std::enable_if_t<is_iterator_v<FilterIter>>
// > {};




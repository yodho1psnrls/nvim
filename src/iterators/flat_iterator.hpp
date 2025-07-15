#pragma once

#include "../meta_func.hpp"

// #include "view.hpp"
#include <ranges> // default_sentinel_t

#include <stdexcept>
#include <type_traits>
#include <iterator>
// #include <utility>      // std::declval

// TODO:
// (*) Do multidimentional flattening typedef
//      that just recursively stacks __flat_iterator
//      by constexpr N number of times
// (*) Make a specialization for __flat_iterator<tensor_iterator>

// TODO: Fix flatten to work recursively
// the problem is the construction of the end iterator
// because it has to get the end->begin(), which is UB

// NOTE: References:
// https://stackoverflow.com/questions/3623082/flattening-iterator
// https://stackoverflow.com/questions/11454825/how-to-flatten-iterators-of-nested-containers?rq=3

// Basicly if you have for example a
// std::vector<std::vector<T>> vec and
// you give it vec.begin() and vec.end()
// it will convert it from a range of 
// vectors to a flattened range of the
// underlying types

// NOTE: For recursive flattening, use make_flat_iterator helper function
// instead of doing FlatIterator(FlatIterator(...)), because that will
// just call the copy constructor and flatten it only once

// NOTE: Flattens a matrix iterator into flat vector iterator,
// you need to give the current row iterator and the end iterator
// (so it doesnt overshoot when the last row happens to be empty)
template <typename RowIter, typename Sentinel = RowIter>
class FlatIterator;

template <typename IT>
inline FlatIterator<IT> make_flat_iterator(const IT& iter) {
  static_assert(is_iterator_v<IT>);
  return FlatIterator<IT, IT>(iter);
}

template <typename IT, typename S>
inline FlatIterator<IT, S> make_flat_iterator(const IT& iter, const S& end) {
  static_assert(is_iterator_v<IT>);
  // static_assert(std::sentinel_for<S, IT>);
  return FlatIterator<IT, S>(iter, end);
}


// namespace std {
//
// template <typename IT>
// inline typename std::iterator_traits<FlatIterator<IT>>::difference_type
// distance(const FlatIterator<IT>& first, const FlatIterator<IT>& last) {
//   // NOTE: In case that we have a random access specialization
//   // if constexpr (std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<FlatIterator<IT>>::iterator_category>)
//   //  return last - first;
//   // else {
//   using difference_type = typename std::iterator_traits<FlatIterator<IT>>::difference_type;
//   difference_type n = 0;
//   IT fir = first.base();
//   while(fir != last.base()) {
//     n += fir->size();
//     ++fir;
//   }
//   return n;
// }
//
// }


// TODO: Fix flatten to work recursively
// the problem is the construction of the end iterator
// because it has to get the end->begin(), which is UB

// template <typename Cont>
// inline auto flatten(Cont&& cont) {
//  using C = std::remove_reference_t<Cont>;
//
//  static_assert(is_range_v<C>);
//  static_assert(is_range_v<typename C::value_type>);
//
//   return View(
//     make_flat_iterator(std::forward<Cont>(cont).begin()),
//     make_flat_iterator(std::forward<Cont>(cont).end())
//   );
// }

template <typename Cont>
inline auto flatten(Cont&& cont) {
  using C = std::remove_reference_t<Cont>;

  // static_assert(is_range_v<C>);
  // static_assert(is_range_v<typename C::value_type>);

  // return View(
  //   make_flat_iterator(std::forward<Cont>(cont).begin(), std::forward<Cont>(cont).end()),
  //   // make_flat_iterator(std::forward<Cont>(cont).end())
  //   make_flat_iterator(std::forward<Cont>(cont).end(), std::forward<Cont>(cont).end())
  // );

  return std::ranges::subrange(
    make_flat_iterator(cont.begin(), cont.end()),
    std::default_sentinel_t()
  );

}


template <typename RowIter, typename Sentinel>
class FlatIterator {

  static_assert(is_iterator_v<RowIter>);
  // static_assert(is_container_v<typename std::iterator_traits<RowIter>::value_type>);
  static_assert(is_range_v<typename std::iterator_traits<RowIter>::value_type>);

  using ThisType = FlatIterator<RowIter>;
  using RowType = typename std::iterator_traits<RowIter>::value_type; 

  // using ValIter = 
  //   _if_t<is_const_iterator_v<RowIter>, 
  //     typename RowType::const_iterator,
  //     typename RowType::iterator>;

  // BUG: With RowIter == RefAssignIterator<std::vector<int>>
  // using ValIter = decltype(std::declval<RowType>().begin());
  // using ValIter = decltype((*std::declval<RowIter>()).begin());
  using ValIter = std::remove_cvref_t<decltype((*std::declval<RowIter>()).begin())>;

  RowIter _row_it;
  [[no_unique_address]] Sentinel _end_row_it;
  ValIter _val_it;

  // using base_category = base_t<
  //   typename std::iterator_traits<RowIter>::iterator_category,
  //   typename std::iterator_traits<ValIter>::iterator_category>;

public:

  // using iterator_category = base_t<std::bidirectional_iterator_tag, base_category>;
  
  using iterator_category = std::forward_iterator_tag;
  using difference_type = typename std::iterator_traits<ValIter>::difference_type;
  using value_type = typename std::iterator_traits<ValIter>::value_type;
  using reference = typename std::iterator_traits<ValIter>::reference;
  using pointer = typename std::iterator_traits<ValIter>::pointer;

// private:
  // difference_type _row_size;
public:

  explicit FlatIterator(const RowIter& vec_it)
  : _row_it(vec_it), _end_row_it(vec_it), _val_it(vec_it->begin()) {}
  
  FlatIterator(const RowIter& vec_it, const Sentinel& end)
  : _row_it(vec_it), _end_row_it(end), _val_it(vec_it->begin()) {
    while(_row_it->empty() && _row_it != end) {
      ++_row_it;
    }
    _val_it = _row_it->begin();
  }

  FlatIterator(const RowIter& vec_it, const ValIter& val_it)
  : _row_it(vec_it), _end_row_it(vec_it), _val_it(val_it) {
    if (vec_it->empty())
      throw std::invalid_argument("you cannot give the flat value iterator, if the upper row iterator is empty");
  }
  
  FlatIterator(const RowIter& vec_it, const Sentinel& end, const ValIter& val_it)
  : _row_it(vec_it), _end_row_it(end), _val_it(val_it) {
    if (vec_it->empty())
      throw std::invalid_argument("you cannot give the flat value iterator, if the upper row iterator is empty");
  }

  template <typename IT, typename = std::enable_if_t<std::is_constructible_v<RowIter, IT>>>
  FlatIterator(const FlatIterator<IT>& other)
  // : FlatIterator(other.base(), other.sentinel(), other.flat()) {}
  : _row_it(other.base()), _end_row_it(other.sentinel()), _val_it(other.flat()) {}

  using base_type = RowIter;
  using sentinel_type = Sentinel;
  using flat_type = ValIter;

  [[nodiscard]] const RowIter& base() const { return _row_it; }
  [[nodiscard]] const Sentinel& sentinel() const { return _end_row_it; }
  [[nodiscard]] const ValIter& flat() const { return _val_it; }

  reference operator*() const { return *_val_it; }
  pointer operator->() const { return _val_it.operator->(); }


  // bool operator==(const ThisType& other) const { return _val_it == other._val_it; }
  // bool operator!=(const ThisType& other) const { return _val_it != other._val_it; }

  bool operator==(const ThisType& other) const {
    if (_end_row_it != other._end_row_it)
      throw std::invalid_argument("Cannot compare two iterators with different end");
    return _row_it == other._row_it && _val_it == other._val_it;
  }

  bool operator!=(const ThisType& other) const {
    if (_end_row_it != other._end_row_it)
      throw std::invalid_argument("Cannot compare two iterators with different end");
    return _row_it != other._row_it || _val_it != other._val_it;
  }

  
  bool operator==(std::default_sentinel_t) const {
    return _row_it == _end_row_it;
  }

  bool operator!=(std::default_sentinel_t) const {
    return _row_it != _end_row_it;
  }

  friend bool operator==(std::default_sentinel_t, const FlatIterator& rhs) {
    return rhs._row_it == rhs._end_row_it;
  }

  friend bool operator!=(std::default_sentinel_t, const FlatIterator& rhs) {
    return rhs._row_it != rhs._end_row_it;
  }


  ThisType& operator++() {
    ++_val_it;

    while(_row_it != _end_row_it && _val_it == _row_it->end()) {
      ++_row_it;
      _val_it = _row_it->begin();
    }

    // ++_val_it;
    //
    // if (_val_it == _row_it->end()) 
    //  do {
    //    if (_row_it != _end_row_it)
    //      ++_row_it;
    //  } while(_row_it != _end_row_it && _row_it->empty());
    //
    // if (_row_it != _end_row_it)
    //  _val_it = _row_it->begin();

    return *this;
  } 

  ThisType operator++(int) {
    ThisType temp(*this);
    ++(*this);
    return temp;
  }

};


// Flattens a matrix iterator into flat vector iterator
/*template <typename RowIter>
class FlatIterator {

  static_assert(is_iterator_v<RowIter>);
  // static_assert(is_container_v<typename std::iterator_traits<RowIter>::value_type>);
  static_assert(is_range_v<typename std::iterator_traits<RowIter>::value_type>);

  using ThisType = FlatIterator<RowIter>;
  using RowType = typename std::iterator_traits<RowIter>::value_type; 

  // using ValIter = 
  //   _if_t<is_const_iterator_v<RowIter>, 
  //     typename RowType::const_iterator,
  //     typename RowType::iterator>;

  // BUG: With RowIter == RefAssignIterator<std::vector<int>>
  // using ValIter = decltype(std::declval<RowType>().begin());
  // using ValIter = decltype((*std::declval<RowIter>()).begin());
  using ValIter = std::remove_cvref_t<decltype((*std::declval<RowIter>()).begin())>;

  RowIter _row_it;
  ValIter _val_it;

  // using base_category = base_t<
  //   typename std::iterator_traits<RowIter>::iterator_category,
  //   typename std::iterator_traits<ValIter>::iterator_category>;

public:

  // using iterator_category = base_t<std::bidirectional_iterator_tag, base_category>;
  
  using iterator_category = most_base_t<
    std::bidirectional_iterator_tag,
    typename std::iterator_traits<RowIter>::iterator_category,
    typename std::iterator_traits<ValIter>::iterator_category
  >;

  using difference_type = typename std::iterator_traits<ValIter>::difference_type;
  using value_type = typename std::iterator_traits<ValIter>::value_type;
  using reference = typename std::iterator_traits<ValIter>::reference;
  using pointer = typename std::iterator_traits<ValIter>::pointer;

// private:
  // difference_type _row_size;
public:

  explicit FlatIterator(const RowIter& vec_it)
    : _row_it(vec_it), _val_it((*vec_it).begin()) {
    // if ((*_row_it).empty())
    //   throw std::out_of_range("Cannot have empty container for flattening");
  }

  FlatIterator(const RowIter& vec_it, const ValIter& val_it)
    : _row_it(vec_it), _val_it(val_it) {
    // if ((*_row_it).empty() && val_it != std::prev((*_row_it).end()))
    //   throw std::out_of_range("Cannot have empty container for flattening");
  }


  template <typename IT, typename = std::enable_if_t<std::is_constructible_v<RowIter, IT>>>
  FlatIterator(const FlatIterator<IT>& other)
  : FlatIterator(other.base(), other.flat()) {}

  const RowIter& base() const { return _row_it; }
  const ValIter& flat() const { return _val_it; }


  reference operator*() const { return *_val_it; }
  pointer operator->() const { return _val_it.operator->(); }

  // bool operator==(const ThisType& other) const { return _val_it == other._val_it; }
  // bool operator!=(const ThisType& other) const { return _val_it != other._val_it; }

  bool operator==(const ThisType& other) const {
    return _row_it == other._row_it && _val_it == other._val_it;
  }

  bool operator!=(const ThisType& other) const {
    return _row_it != other._row_it || _val_it != other._val_it;
  }


  ThisType& operator++() {
    if ((*_row_it).empty())
      throw std::out_of_range("Cannot have empty container for flattening");

    ++_val_it;

    if(_val_it == (*_row_it).end()) {
      ++_row_it;
      _val_it = (*_row_it).begin();
    }

    return *this;
  } 

  ThisType operator++(int) { ThisType temp(*this); ++(*this); return temp; }


// ---------------------- Legacy Bidirectional Iterator ------------------------- //


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator--() {
    if(_val_it == (*_row_it).begin()) {
      --_row_it;
      _val_it = (*_row_it).end();
    }

    if ((*_row_it).empty())
      throw std::out_of_range("Cannot have empty container for flattening");

    --_val_it;

    return *this;
  } 

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator--(int) { ThisType temp(*this); --(*this); return temp; }


};*/


/*
template <typename RowIter>
class __flat_square_iterator {

  using ThisType = __flat_square_iterator<RowIter>;
  using RowType = typename RowIter::value_type; 

  using ValIter = 
    _if_t<is_const_iterator<RowIter>, 
      typename RowType::const_iterator, 
      typename RowType::iterator>;

  RowIter _row_it;
  ValIter _val_it;



public:

  using iterator_category = base_t<
    typename std::iterator_traits<RowIter>::iterator_category,
    typename std::iterator_traits<ValIter>::iterator_category>;

  using difference_type = typename std::iterator_traits<ValIter>::difference_type;
  using value_type = typename std::iterator_traits<ValIter>::value_type;
  using reference = typename std::iterator_traits<ValIter>::reference;
  using pointer = typename std::iterator_traits<ValIter>::pointer;

private:
  difference_type _row_size;
public:

  __flat_square_iterator(const RowIter& vec_it, const difference_type& row_size)
    : _row_it(vec_it), _val_it((*vec_it).begin()), _row_size(row_size) {}

  __flat_square_iterator(const RowIter& vec_it, const ValIter& val_it, const difference_type& row_size)
    : _row_it(vec_it), _val_it(val_it) , _row_size(row_size) {}

  
  reference operator*() const { return *_val_it; }
  pointer operator->() const { return _val_it.operator->(); }

  bool operator==(const ThisType& other) const { return _val_it == other._val_it; }
  bool operator!=(const ThisType& other) const { return _val_it != other._val_it; }


  ThisType& operator++() {
    ++_val_it;

    if(_val_it == (*_row_it).end()) {
      ++_row_it;
      _val_it = (*_row_it).begin();
    }

    return *this;
  } 

  ThisType operator++(int) { auto temp(*this); ++(*this); return temp; }


// ---------------------- Legacy Bidirectional Iterator ------------------------- //


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator--() {
    if(_val_it == (*_row_it).begin()) {
      --_row_it;
      _val_it = (*_row_it).end();
    }

    --_val_it;

    return *this;
  } 

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator--(int) { auto temp(*this); --(*this); return temp; }


// ---------------------- Legacy Random Access Iterator --------------------------- //


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator+=(difference_type n) {
    n += _val_it - (*_row_it).begin();
    difference_type row_size = (*_row_it).size();
    _row_it += n / row_size;
    _val_it = (*_row_it).begin() + n % row_size;

    return *this;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator-=(difference_type n) {
    n += _val_it - (*_row_it).end();
    difference_type row_size = (*_row_it).size();
    _row_it -= n / row_size;
    _val_it = (*_row_it).begin() - n % row_size;

    return *this;
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator+(const difference_type n) const {
    auto temp(*this); return temp += n; 
  }


  template <typename IC = iterator_category>
  friend std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator+(const difference_type n, const ThisType& cit) {
    return cit + n; 
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator-(difference_type n) const {
    auto temp(*this); return temp -= n; 
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  difference_type>::type operator-(const ThisType& other) const {
    if((*_row_it).size() != (*other._row_it).size())
      return (_row_it - other._row_it) * std::min(*(_row_it).size(), *(other._row_it).size());


    difference_type vn = _val_it - (*_row_it).begin() - (other._val_it - (*other._row_it).begin());
    difference_type rn = _row_it - other._row_it;
    difference_type row_size = (*_row_it).size();

    return rn * row_size + vn;
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  reference>::type operator[](const difference_type n) const { auto temp = *this; return *(temp += n); }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  reference>::type at(const difference_type n) const { auto temp = *this; return *(temp += n); }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator<(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it < other._val_it;

    return _row_it < other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator<=(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it <= other._val_it;

    return _row_it <= other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator>(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it > other._val_it;

    return _row_it > other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator>=(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it >= other._val_it;

    return _row_it >= other._row_it;
  }


};
*/


/*
template <typename RowIter, bool ROWS_SAME_SIZE = false>
class __flat_iterator {

  using ThisType = __flat_iterator<RowIter, ROWS_SAME_SIZE>;
  using RowType = typename RowIter::value_type; 

  using ValIter = 
    _if_t<is_const_iterator<RowIter>, 
      typename RowType::const_iterator, 
      typename RowType::iterator>;

  RowIter _row_it;
  ValIter _val_it;

  using base_category = base_t<
    typename std::iterator_traits<RowIter>::iterator_category,
    typename std::iterator_traits<ValIter>::iterator_category>;


public:

  using iterator_category = _if_t<
    ROWS_SAME_SIZE && std::is_base_of_v<
      std::random_access_iterator_tag, base_category
    >, base_category, std::bidirectional_iterator_tag>;

  using difference_type = typename std::iterator_traits<ValIter>::difference_type;
  using value_type = typename std::iterator_traits<ValIter>::value_type;
  using reference = typename std::iterator_traits<ValIter>::reference;
  using pointer = typename std::iterator_traits<ValIter>::pointer;


  __flat_iterator(const RowIter& vec_it) 
    : _row_it(vec_it), _val_it((*vec_it).begin()) {}

  __flat_iterator(const RowIter& vec_it, const ValIter& val_it)
    : _row_it(vec_it), _val_it(val_it) {}

  
  reference operator*() const { return *_val_it; }
  pointer operator->() const { return _val_it.operator->(); }

  bool operator==(const ThisType& other) const { return _val_it == other._val_it; }
  bool operator!=(const ThisType& other) const { return _val_it != other._val_it; }


  ThisType& operator++() {
    ++_val_it;

    if(_val_it == (*_row_it).end()) {
      ++_row_it;
      _val_it = (*_row_it).begin();
    }

    return *this;
  } 

  ThisType operator++(int) { auto temp(*this); ++(*this); return temp; }


// ---------------------- Legacy Bidirectional Iterator ------------------------- //


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator--() {
    if(_val_it == (*_row_it).begin()) {
      --_row_it;
      _val_it = (*_row_it).end();
    }

    --_val_it;

    return *this;
  } 

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator--(int) { auto temp(*this); --(*this); return temp; }


// ---------------------- Legacy Random Access Iterator --------------------------- //
*/
/*
  template <typename IT = ValIter>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType&>::type operator+=(difference_type n) {
    n += _val_it - (*_row_it).begin();

    if constexpr (ROWS_SAME_SIZE &&
      std::is_base_of_v<std::random_access_iterator_tag,
      typename std::iterator_traits<RowIter>::iterator_category>
    ) {
      difference_type row_size = (*_row_it).size();
      _row_it += n / row_size;
      _val_it = (*_row_it).begin() + n % row_size;
    }
    else {
      while(n > (*_row_it).size()) {
        n -= (*_row_it).size();
        ++_row_it;
      }
      _val_it = (*_row_it).begin() + n;
    }
  }


  template <typename IT = ValIter>
  std::enable_if<std::is_base_of_v<
    std::random_access_iterator_tag, 
    typename std::iterator_traits<IT>::iterator_category>,
  ThisType&>::type operator-=(difference_type n) {
    n += _val_it - (*_row_it).end();
    if constexpr (ROWS_SAME_SIZE &&
      std::is_base_of_v<std::random_access_iterator_tag,
      typename std::iterator_traits<RowIter>::iterator_category>
    ) {
      size_t row_size = (*_row_it).size();
      _row_it -= n / row_size;
      _val_it = (*_row_it).begin() - n % row_size;
    }
    else {
      while(n > (*_row_it).size()) {
        n -= (*_row_it).size();
        ++_row_it;
      }
      _val_it (*_row_it).begin() + n;
    }
  }
*/
/*
  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator+=(difference_type n) {
    n += _val_it - (*_row_it).begin();
    difference_type row_size = (*_row_it).size();
    _row_it += n / row_size;
    _val_it = (*_row_it).begin() + n % row_size;

    return *this;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType&>::type operator-=(difference_type n) {
    n += _val_it - (*_row_it).end();
    difference_type row_size = (*_row_it).size();
    _row_it -= n / row_size;
    _val_it = (*_row_it).begin() - n % row_size;

    return *this;
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator+(const difference_type n) const {
    auto temp(*this); return temp += n; 
  }


  template <typename IC = iterator_category>
  friend std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator+(const difference_type n, const ThisType& cit) {
    return cit + n; 
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  ThisType>::type operator-(difference_type n) const {
    auto temp(*this); return temp -= n; 
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  difference_type>::type operator-(const ThisType& other) const {
    if((*_row_it).size() != (*other._row_it).size())
      return (_row_it - other._row_it) * std::min(*(_row_it).size(), *(other._row_it).size());


    difference_type vn = _val_it - (*_row_it).begin() - (other._val_it - (*other._row_it).begin());
    difference_type rn = _row_it - other._row_it;
    difference_type row_size = (*_row_it).size();

    return rn * row_size + vn;
  }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  reference>::type operator[](const difference_type n) const { auto temp = *this; return *(temp += n); }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  reference>::type at(const difference_type n) const { auto temp = *this; return *(temp += n); }


  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator<(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it < other._val_it;

    return _row_it < other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator<=(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it <= other._val_it;

    return _row_it <= other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator>(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it > other._val_it;

    return _row_it > other._row_it;
  }

  template <typename IC = iterator_category>
  std::enable_if<std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>,
  bool>::type operator>=(const ThisType& other) const {
    if(_row_it == other._row_it)
      return _val_it >= other._val_it;

    return _row_it >= other._row_it;
  }


};
*/


// template <typename RowIter>
// class FlatIterable : public Iterable<__flat_iterator<RowIter>> {
// public:
//  using Iterable<__flat_iterator<RowIter>>::Iterable; // Constructors
// };



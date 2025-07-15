#pragma once
#include "../cont_adaptors/traits.hpp" // is_key_container_v
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../iterators/index_iterator.hpp"
#include "../key_kernel/keys.hpp"

#include <vector>
#include <utility>


template <typename ValKey = size_t>
class KeyJaggedMatPolicy {

  // template <typename KeyCont>
  // using cont_to_key_type = typename KeyCont::key_type;
  // 
  // template <typename KeyCont>
  // using cont_to_key_iter = typename KeyCont::key_iterator;

public:

  static_assert(is_key_handle_v<ValKey>);
  static_assert(std::is_same_v<index_handle_category,
    typename handle_traits<ValKey>::handle_category>);

  using flat_key_type = ValKey;
  using flat_key_iterator = typename handle_traits<ValKey>::iterator;

  using difference_type = typename handle_traits<ValKey>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  using key_type = size_t;
  using key_iterator = index_iterator<size_t>;
  using value_type = std::vector<key_type>;
  
  using row_ranges_cont = std::vector<key_type>;

  using reference = TensorIterable<jump_iterator<flat_key_iterator, JumpByIndexDiff<typename row_ranges_cont::const_iterator>>>;
  using pointer = reference*;
  using iterator = IterableToIterator<reference>;
  
protected:

  void shift(key_iterator key_it, difference_type offset) {
    auto it = _row_ranges.begin() + *key_it;
    ++it;
    for(; it != _row_ranges.end(); ++it)
      *it += offset;
  }

public:

  KeyJaggedMatPolicy()
  : _row_ranges({0}) {}

  // template <typename KeyCont>
  // void reserve_flat(KeyCont& flat, size_type n) {
  //   flat.reserve(n);
  // }

  size_type size() const {
    return _row_ranges.size() - 1;
  }
  
  void clear() {
    _row_ranges.assign({0});
  }

  template <typename KeyCont>
  void reserve(size_type n) {
    _row_ranges.reserve(n + 1);
  }

  key_iterator begin() const {
    return key_iterator(size_type(0));
  }
  
  key_iterator end() const {
    return key_iterator(size_type(size()));
  }

  // template <typename flat_key_iterator>
  // reference<flat_key_iterator> at(key_type key) const {
  //   return reference(flat_key_iterator(key * _n), _n);
  // }
  
  reference at(key_type key) const {
    // return reference<KeyIter>(
    //   KeyIter(_row_ranges[key]),
    //   _row_ranges.cbegin() + key
    // );
    iterator it(0ULL, _row_ranges.cbegin());
    return *(it + key);
  }

  key_iterator insert(key_iterator key_it) {
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, *it);
    return key_it;
  }

  key_iterator insert(key_iterator key_it, size_type num_of_rows) {
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, num_of_rows, *it);
    return key_it;
  }

  key_iterator erase(key_iterator key_it) {
    // auto row_ids = at(*key_it);
    // erase_in(key_it, row_ids.begin(), row_ids.end());
    shift(key_it, -at(*key_it).size());
    _row_ranges.erase(_row_ranges.begin() + *key_it);
    return key_it;
  }

  key_iterator erase(key_iterator from, key_iterator to) {
    auto itb = Iterable(
      _row_ranges.begin() + *from,
      _row_ranges.begin() + *to
    );
    shift(to, *itb.end() - *itb.begin());
    _row_ranges.erase(itb.begin(), itb.end());
    return from;
  }

  void insert_in(key_iterator key_it, flat_key_iterator flat_key) {
  // flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_key) {
    shift(key_it, 1);
    // return flat_key;
  }

  void insert_in(key_iterator key_it, flat_key_iterator flat_key, size_type num_of_vals) {
  // flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_key, size_type num_of_vals) {
    shift(key_it, num_of_vals);
    // return flat_key;
  }

  void erase_in(key_iterator key_it, flat_key_iterator flat_key) {
  // flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_key) {
    shift(key_it, -1);
    // return flat_key;
  }

  void erase_in(key_iterator key_it, flat_key_iterator flat_from, flat_key_iterator flat_to) {
  // flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_from, flat_key_iterator flat_to) {
    shift(key_it, -std::distance(flat_from, flat_to));
    // return flat_from;
  }

private:

  row_ranges_cont _row_ranges;

};



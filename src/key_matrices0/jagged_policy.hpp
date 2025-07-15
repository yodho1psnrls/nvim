#pragma once
#include "../cont_adaptors/traits.hpp" // is_key_container_v
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../iterators/index_iterator.hpp"
#include <vector>


// template <typename KeyContainer>
class KeyJaggedMatPolicy {

  template <typename KeyCont>
  using cont_to_key_type = typename KeyCont::key_type;
  
  template <typename KeyCont>
  using cont_to_key_iter = typename KeyCont::key_iterator;

public:

  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  
  using key_type = size_t;
  using key_iterator = index_iterator<size_t>;
  using value_type = std::vector<key_type>;
  
  using row_ranges_cont = std::vector<difference_type>;

  template <typename FlatKeyIter>
  using reference = TensorIterable<jump_iterator<FlatKeyIter, JumpByIndexDiff<typename row_ranges_cont::const_iterator>>>;
  
  template <typename FlatKeyIter>
  using pointer = reference<FlatKeyIter>*;

  template <typename FlatKeyIter>
  using iterator = IterableToIterator<reference<FlatKeyIter>>;
 
  KeyJaggedMatPolicy()
  : _row_ranges({0}) {}

  template <typename KeyCont>
  void clear(KeyCont& flat) {
    flat.clear();
    _row_ranges.assign({0});
  }

  template <typename KeyCont>
  void reserve(KeyCont&, size_type n) {
    _row_ranges.reserve(n + 1);
  }
  
  // template <typename KeyCont>
  // void reserve_flat(KeyCont& flat, size_type n) {
  //   flat.reserve(n);
  // }

  template <typename KeyCont>
  size_type size(const KeyCont&) const {
    static_assert(is_key_container_v<KeyCont>);
    return _row_ranges.size() - 1;
  }

  template <typename KeyCont>
  key_iterator begin(const KeyCont&) const {
    static_assert(is_key_container_v<KeyCont>);
    return key_iterator(size_type(0));
  }
  
  template <typename KeyCont>
  key_iterator end(const KeyCont& cont) const {
    static_assert(is_key_container_v<KeyCont>);
    return key_iterator(size_type(this->size(cont)));
  }

  // template <typename flat_key_iterator>
  // reference<flat_key_iterator> at(key_type key) const {
  //   return reference(flat_key_iterator(key * _n), _n);
  // }
  
  template <typename KeyCont>
  reference<cont_to_key_iter<KeyCont>> at(const KeyCont&, key_type key) const {
    static_assert(is_key_container_v<KeyCont>);
    using KeyIter = cont_to_key_iter<KeyCont>;
    // return reference<KeyIter>(
    //   KeyIter(_row_ranges[key]),
    //   _row_ranges.cbegin() + key
    // );
    iterator<KeyIter> it(0ULL, _row_ranges.cbegin());
    return *(it + key);
  }

  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it) {
    static_assert(is_key_container_v<KeyCont>);
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, *it);
    return key_it;
  }

  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it, size_type num_of_rows) {
    static_assert(is_key_container_v<KeyCont>);
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, num_of_rows, *it);
    return key_it;
  }

  template <typename KeyCont>
  key_iterator erase(KeyCont& flat, key_iterator key_it) {
    static_assert(is_key_container_v<KeyCont>);
    flat.erase(
      at(flat, *key_it).begin(),
      at(flat, *key_it).end()
    );
    _row_ranges.erase(_row_ranges.begin() + *key_it);
    return key_it;
  }

  template <typename KeyCont>
  key_iterator erase(KeyCont& flat, key_iterator from, key_iterator to) {
    static_assert(is_key_container_v<KeyCont>);
    flat.erase(
      at(flat, *from).begin(),
      at(flat, *to).begin()
    );
    _row_ranges.erase(
      _row_ranges.begin() + *from,
      _row_ranges.begin() + *to
    );
    return from;
  }


  template <typename KeyCont>
  typename KeyCont::key_iterator insert_in(KeyCont& flat, key_iterator key_it, typename KeyCont::key_iterator flat_key) {
    // auto it = _row_ranges.erase(row_it.policy().base(), row_it.policy().base());
    auto it = _row_ranges.begin() + *key_it;
    ++it;
    for(; it != _row_ranges.end(); ++it)
      ++(*it);
    return flat.insert(flat_key);
  }

  template <typename KeyCont>
  typename KeyCont::key_iterator insert_in(KeyCont& flat, key_iterator key_it, typename KeyCont::key_iterator flat_key, size_type num_of_vals) {
    auto it = _row_ranges.begin() + *key_it;
    ++it;
    for(; it != _row_ranges.end(); ++it)
      *it += num_of_vals;
    return flat.insert(flat_key, num_of_vals);
  }

  template <typename KeyCont>
  typename KeyCont::key_iterator erase_in(KeyCont& flat, key_iterator key_it, typename KeyCont::key_iterator flat_key) {
    auto it = _row_ranges.begin() + *key_it;
    ++it;
    for(; it != _row_ranges.end(); ++it)
      --(*it);
    return flat.erase(flat_key);
  }

  template <typename KeyCont>
  typename KeyCont::key_iterator erase_in(KeyCont& flat, key_iterator key_it, typename KeyCont::key_iterator flat_from, typename KeyCont::key_iterator flat_to) {
    auto it = _row_ranges.begin() + *key_it;
    ++it;
    for(; it != _row_ranges.end(); ++it)
      *it -= std::distance(flat_from, flat_to);
    return flat.erase(flat_from, flat_to);
  }

private:

  row_ranges_cont _row_ranges;

};



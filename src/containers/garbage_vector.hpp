#pragma once

#include "../meta_func.hpp"
#include "order_traits.hpp"

#include <vector>
// #include <memory>
// #include <utility>
// #include <iterator>
#include <type_traits>

// The container of bools should be of the same type

// NOTE: Generalize this wrapper for more containers
// that only vector-like and add garbage policies that will
// be chosed depending on the possibilities of the key type
// For example, if it is an index, then use a bit_mast like std::vector<bool>,
// if it is comparable with std::less<>, then choose a std::set<key> or std::priority_queue
// if it is not any of the above, then choose std::queue<key> or std::stack<key>
// or std::unordered_set<key>(if hashable)

// TODO: Just Make it to always insert at free places, no matter the given position
// it will only insert at that position, if there are no empty places

template <typename VecType>
class GarbageVector 
: public VecType {

  std::vector<bool> _erased_mask;
  size_t _erased_count {};

public:

  using base_type = VecType;
  
  using value_type = typename base_type::value_type;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;

  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;
  
  // using base_type::base_type;
  
  template <typename IterType>
  GarbageVector(IterType first, IterType last)
  : base_type(first, last)
  // , _erased_mask(std::distance(first, last), false) {}
  , _erased_mask(base_type::size(), false) {}
  
  GarbageVector(std::initializer_list<value_type> l)
  : base_type(l), _erased_mask(l.size(), false) {}

  GarbageVector(size_type n, const value_type& val)
  : base_type(n, val), _erased_mask(n, false) {}

  template <typename IterType>
  void assign(IterType first, IterType last) {
    base_type::assign(first, last);
    _erased_mask.assign(base_type::size(), false);
  }
  
  void assign(std::initializer_list<value_type> l) {
    base_type::assign(l);
    _erased_mask.assign(l.size(), false);
  }

  void assign(size_type n, const value_type& val) {
    base_type::assign(n, val);
    _erased_mask.assign(n, false);
  }

protected:

  iterator _nonconst_iter(const_iterator cit) {
    return base_type::begin() + (cit - base_type::cbegin());
  }

public:

  bool is_compact() const {
    return !_erased_count;
  }
  
  void compact() {
    if (is_compact())
      return;

    // for(auto it = base_type::rbegin(); it != base_type::rend(); ++it)
    //  if (is_erased())
    //    base_type::erase(it.base());

    for(difference_type i = base_type::size() - 1; i != -1; --i)
      if(is_erased(i)) {
        base_type::erase(base_type::cbegin() + i);
        // _erased_mask[i] = false; // They should be separate
      }

    _erased_mask.assign(_erased_mask.size(), false);
    _erased_count = 0;
  }

  size_t erased_size() const {
    return _erased_count;
  }

  bool is_erased(size_type id) const {
    return _erased_mask[id];
  }
  
  bool is_erased(const_iterator it) const {
    return _erased_mask[it - base_type::cbegin()];
  }

  template <typename ...Args>
  iterator emplace(const_iterator position, Args&&... args) {

  }

  iterator insert(const_iterator position, const value_type& val) {
    if (is_compact())
      return base_type::insert(position, val);
    
    difference_type i = 0;
    while(!_erased_mask[i])
      ++i;

    _erased_mask[i] = false;
    --_erased_count;
    iterator it = base_type::begin() + i;
    *it = val;
    return it;
  }
  
  iterator insert(const_iterator position, value_type&& val) {
    if (is_compact())
      return base_type::insert(position, val);
    
    difference_type i = 0;
    while(!_erased_mask[i])
      ++i;

    _erased_mask[i] = false;
    --_erased_count;
    iterator it = base_type::begin() + i;
    *it = std::move(val);
    return it;
  }

  // NOTE: For the range insert methods we need another helper method
  // that will compact the rest of the values that are not in the position
  // that we insert, so for example if we have a vector of
  // **-*-*--*-**-* and we insert
  // here  ^  , then we want it to look like this
  // ****------**** and then we insert our new values
  // here^

  // pop_swap_vector<int> vec(6);
  // vec.insert(vec.begin() + 2, 11, 111); // BUG: This Breaks
  iterator insert(const_iterator position, size_type n, const value_type& val) {

  }

  template <typename InputIterator, typename =
    std::enable_if_t<is_iterator_v<InputIterator>>>
  iterator insert(const_iterator position, InputIterator first, InputIterator last) {

  }

  iterator insert(const_iterator position, std::initializer_list<value_type> l) {

  }


  iterator erase(const_iterator position) {
    difference_type id = position - base_type::cbegin();
    _erased_mask[id] = true;
    ++_erased_count;
    // return std::next(_nonconst_iter(position));
    return base_type::begin() + id + 1;
  }

  iterator erase(const_iterator first, const_iterator last) {
    difference_type f_id = first - base_type::cbegin();
    difference_type l_id = last - base_type::cbegin();
    
    _erased_count += l_id - f_id;

    while(f_id != l_id) {
      _erased_mask[f_id] = true;  // BUG:
      ++f_id;
    }

    // return _nonconst_iter(last);
    return base_type::begin() + l_id;
  }

};






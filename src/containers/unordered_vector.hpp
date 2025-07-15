#pragma once

#include "../meta_func.hpp"
// #include "../iterators/assign_iterator.hpp"
// #include "shift_policies.hpp"

// #include <vector>
// #include <memory>
// #include <utility>
#include <iterator>
#include <type_traits>
#include <algorithm>

// https://www.reddit.com/r/cpp_questions/comments/s8arid/how_do_i_efficiently_insert_elements_of_a_vector/?rdt=44170


// NOTE:
// std::vector is ordered by insertion but
// this is slow, because it moves a lot of elements
// so this is a unordered vector, which does not
// preserve the elements insertion order
// it uses Swap and Pop strategy to resize
// the container, inserting and erasing a single
// element is O(1) and for n elements its O(min(n, pos_to_end))
template <typename BaseVec>
class UnorderedVector
: public BaseVec {
public:

  // using base_type = std::vector<T, Alloc>;
  using base_type = BaseVec;
  static_assert(is_container_v<BaseVec>);
  static_assert(is_vector_container_v<BaseVec>);
  // static_assert(std::is_base_of_v<std::random_access_iterator_tag,
  //   typename std::iterator_traits<typename BaseVec::iterator>::iterator_category>);

  UnorderedVector(const base_type& base) : base_type(base) {}
  using base_type::base_type;

  using typename base_type::value_type;
  using typename base_type::size_type;
  using typename base_type::difference_type;
  using typename base_type::iterator;
  using typename base_type::const_iterator;

  // static_assert(std::is_base_of_v<std::random_access_iterator_tag,
  //   typename std::iterator_traits<iterator>::iterator_category>);
 
  // static_assert(std::is_same_v<T, value_type>);

public:

  base_type& base() { return static_cast<base_type&>(*this); }
  const base_type& base() const { return static_cast<const base_type&>(*this); }

  iterator insert(const_iterator position, const value_type& val) {
    difference_type id = position - this->cbegin();
    this->resize(this->size() + 1);
    if constexpr (std::is_move_assignable_v<value_type>)
      this->back() = std::move((*this)[id]);
    else
      this->back() = (*this)[id];
    (*this)[id] = val;
    return this->begin() + id;
  }
  
  iterator insert(const_iterator position, value_type&& val) {
    difference_type id = position - this->cbegin();
    this->resize(this->size() + 1);
    if constexpr (std::is_move_assignable_v<value_type>)
      this->back() = std::move((*this)[id]);
    else
      this->back() = (*this)[id];
    (*this)[id] = std::move(val);
    return this->begin() + id;
  }


  iterator insert(const_iterator position, size_type n, const value_type& val) {
    difference_type id = position - base_type::cbegin();
    iterator prev_end = base_type::insert(base_type::end(), n, val);
    iterator pos = base_type::begin() + id;
    std::swap_ranges(prev_end, base_type::end(), pos);
    return pos;
  }
  
  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  iterator insert(const_iterator position, IterType first, IterType last) {
    static_assert(std::is_convertible_v<
      typename std::iterator_traits<IterType>::value_type,
      value_type>
    );
    difference_type id = position - base_type::cbegin();
    iterator prev_end = base_type::insert(base_type::end(), first, last);
    iterator pos = base_type::begin() + id;
    std::swap_ranges(prev_end, base_type::end(), pos);
    return pos;
  }

  iterator insert(const_iterator position, std::initializer_list<value_type> l) {
    return insert(position, l.begin(), l.end());
  }


  iterator erase(const_iterator position) {
    iterator pos = base_type::erase(position, position);
    // iterator back_it = std::prev(base_type::end());
    *pos = std::move(base_type::back());
    base_type::pop_back();
    return pos;
  }

  iterator erase(const_iterator first, const_iterator last) {
    difference_type id = first - base_type::cbegin();
    difference_type n = std::distance(first, last);
    std::copy(base_type::end() - n, base_type::end(), base_type::begin() + id);
    base_type::erase(base_type::end() - n, base_type::end());
    return base_type::begin() + id;
    
    // difference_type id = first - base_type::cbegin();
    // difference_type n = std::distance(first, last);
    // std::copy(base_type::begin() + id + n, base_type::end(), base_type::begin() + id);
    // base_type::erase(base_type::end() - n, base_type::end());
    // return base_type::begin() + id;
  }

  template <typename ...Args>
  iterator emplace(const_iterator position, Args&&... args) {
    static_assert(std::is_constructible_v<value_type, Args...>);
    // iterator pos = new_pos(position);
    // base_type::push_back(std::move(*pos));
    // *pos = value_type(std::forward<Args>(args)...);
    // return pos;
    difference_type id = position - base_type::cbegin();
    iterator prev_end = base_type::emplace(base_type::end(), std::forward<Args>(args)...);
    iterator pos = base_type::begin() + id;
    std::swap(*pos, *prev_end);
    return pos;
  }


};



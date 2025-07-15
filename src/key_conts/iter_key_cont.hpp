#pragma once

#include "../iterators/iterator_iterator.hpp"
#include "../iterators/transform_iterator.hpp"
#include "../utilities.hpp"

#include <stdexcept>
#include <iterator>


// Any Container with stability of its iterators


// The Container should have stability of
//  references, pointers and iterators !!
// examples from stl:
//  deque, list, forward_list
template <typename Container>
class IterKeyCont {

  Container _cont;

public:

  //using Container = std::vector<T, Alloc>;
  // using container = Container;
  using base_type = Container;

  IterKeyCont(const Container& cont)
  : _cont(cont) {}

  template <typename ...Args>
  IterKeyCont(Args&&... args)
  : _cont(std::forward<Args>(args)...) {}
  
  IterKeyCont(const std::initializer_list<typename Container::value_type>& list)
  : _cont(list) {}
 
  // TODO: move constructor from vector to hvector
  
  // NOTE:
  // IterKeyCont(const Container& cont) : Container(cont) {}
  // IterKeyCont(Container&& cont) : Container(cont) {}
  // operator const Container&() const {
  //   return static_cast<const Container&>(*this);
  // }
  // operator Container&&() {
  // Is this even correct ?
  //   return static_cast<Container&&>(std::move(*this));
  // }


  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  
  using difference_type = typename Container::difference_type;
  using size_type = typename Container::size_type;
  using allocator_type = typename base_type::allocator_type;

  using key_type = typename Container::const_iterator;
  using key_iterator = iterator_iterator<key_type>;
  // using key_type = typename Container::const_pointer;
  // using key_iterator = transform_iterator<index_iterator<typename Container::const_iterator>, IterToPtr>;
  // using key_reverse_iterator = std::reverse_iterator<key_iterator>;

  using iterator = key_iterator;
  using const_iterator = key_iterator;
  // using reverse_iterator = key_reverse_iterator;
  // using const_reverse_iterator = key_reverse_iterator;


  key_iterator begin() const { return key_iterator(_cont.begin()); }
  key_iterator end() const { return key_iterator(_cont.end()); }

  // TODO: This would need SFINAE !!!
  // key_reverse_iterator rbegin() const { return _cont.end(); }
  // key_reverse_iterator rend() const { return _cont.begin(); }
 

  bool empty() const { return _cont.empty(); }
  size_type size() const { return _cont.size(); }
  void clear() { _cont.clear(); }

  const base_type& base() const {
    return _cont;
  }

  key_iterator find(key_type key) const {
    return key_iterator(key);
  }


  key_iterator insert(key_iterator key_it, const value_type& val = {}) {
    // if (!contains(*key_it))
    //   throw std::invalid_argument("Inserting by key, should be between already contained keys !");
    return _cont.insert(*key_it, val);
  }

  key_iterator erase(key_iterator key) {
    // if (!contains(*key_it))
    //   throw std::invalid_argument("Erasing by key, should be for an already contained key !");
    return _cont.erase(key);
  }


  reference operator[](key_type key) {
    // we cast the const_iterator (key) to iterator,
    //  by just erasing an empty range
    return *_cont.erase(key, key);
    // return const_cast<Container::reference>(*key);
  }

  const_reference operator[](key_type key) const {
    return *key;
  }
  
  reference at(key_type key) {
    // we cast the const_iterator (key) to iterator,
    //  by just erasing an empty range
    return *_cont.erase(key, key);
    // return const_cast<Container::reference>(*key);
  }

  const_reference at(key_type key) const {
    return *key;
  }


};

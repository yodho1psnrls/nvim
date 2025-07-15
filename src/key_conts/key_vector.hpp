#pragma once

#include "../iterators/index_iterator.hpp"
#include "../iterators/iterable.hpp"
#include "../meta_func.hpp"
// #include "keys.hpp"
#include "../keys/keys.hpp"
#include <type_traits>
#include <iterator> // reverse_iterator

//   NOTE:
//   Matrix<KeyVector<std::vector<int>, edge_key>> mat1(0, 3);
//   mat1.insert(mat1.end(), {1, 2, 3}); // This fails, when
//   // when the index_handle keys have explicit convertion
//   // operator to another integer type


// template <typename Key, typename T, typename VectorType = std::vector<T>>
// template <typename VectorType, typename Key = typename VectorType::size_type>
template <typename VectorType, typename KeyTag = void>
// template <typename VectorType>
class KeyVector {

  static_assert(is_container_v<VectorType>);
  static_assert(is_vector_container_v<VectorType>);
  // static_assert(std::is_default_constructible_v<typename VectorType::value_type>);

  VectorType _vec;

public:

  using base_type = VectorType;
  
  using difference_type = typename base_type::difference_type;
  using size_type = typename base_type::size_type;
  using allocator_type = typename base_type::allocator_type;

 
	// NOTE:
  // static_assert(!std::is_same_v<value_type, size_type>);

  // using key_type = index_handle<size_type, KeyTag>;
  // using key_iterator = index_iterator<size_type, key_type>;

  // using key_type = size_type;
  // using key_iterator = index_iterator<size_type>;

	using key_type = _if_t<
		std::is_same_v<KeyTag, void>,
		size_type,
		TaggedType<size_type, KeyTag>
	>;
	
	using key_iterator = _if_t<
		std::is_same_v<KeyTag, void>,
		index_iterator<size_type>,
		TaggedIterator<index_iterator<size_type>, KeyTag>
	>;

  using key_reverse_iterator = std::reverse_iterator<key_iterator>;
 

  using value_type = typename base_type::value_type;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using value_iterator = typename base_type::iterator;
  using const_value_iterator = typename base_type::const_iterator;

  using iterator = key_iterator;
  using const_iterator = key_iterator;
  using reverse_iterator = key_reverse_iterator;
  using const_reverse_iterator = key_reverse_iterator;


  const base_type& base() const { return _vec; }
  base_type& base() { return _vec; }

  
  template <typename ...Args, typename = std::enable_if_t<
    std::is_constructible_v<base_type, Args...>>>
  KeyVector(Args&&... args)
  : _vec(std::forward<Args>(args)...) {}

  // KeyVector(std::initializer_list<value_type> l)
  // : _vec(l) {}


  key_iterator begin() const {
    return key_iterator(key_type(0));
  }

  key_iterator end() const {
    return key_iterator(key_type(size()));
  }
  
  key_reverse_iterator rbegin() const {
    return key_reverse_iterator(end());
  }

  key_reverse_iterator rend() const {
    return key_reverse_iterator(begin());
  }

  InputIterable<key_iterator> keys() const {
    return {
      begin(),
      end()
    };
  }

  Iterable<const_iterator> values() const {
    return {
      _vec.cbegin(),
      _vec.cend()
    };
  }
  
  Iterable<iterator> values() {
    return {
      _vec.begin(),
      _vec.end()
    };
  }

  size_type size() const { return _vec.size(); }
  bool empty() const { return _vec.empty(); }
  void clear() { _vec.clear(); }

  key_iterator find(key_type key) const {
    return key_iterator(key);
  }

  bool contains(key_type key) const {
    return 0 < key && key < _vec.size();
  }

  void reserve(size_type n) {
    _vec.reserve(n);
  }

  void resize(size_type n) {
    _vec.resize(n);
  }

  void push_back(const value_type& val = value_type()) {
    _vec.push_back(val);
  }
  
  void pop_back() {
    _vec.pop_back();
  }


  // key_iterator insert(key_iterator key_it, const_reference val = {}) {
  // template <typename T = value_type>
  // key_iterator insert(key_iterator key_it, const T& val = T()) {
  // key_iterator insert(key_iterator key_it) {
  key_iterator insert(key_iterator key_it, const value_type& val = value_type()) {
    // _vec.insert(_vec.begin() + key, {}); // BUG:
    // _vec.insert(_vec.begin() + *key_it, val);
    // _vec.insert(_vec.begin() + difference_type(*key_it), value_type());
    _vec.insert(_vec.begin() + difference_type(*key_it), val);
    return key_it;
  }

  // template <typename T = value_type, typename = std::enable_if_t<std::is_lvalue_reference_v<T>>>
  // key_iterator insert(key_iterator key_it, T&& val) {
  key_iterator insert(key_iterator key_it, value_type&& val) {
    // _vec.insert(_vec.begin() + key, {}); // BUG:
    // _vec.insert(_vec.begin() + *key_it, std::move(val));
    _vec.insert(_vec.begin() + difference_type(*key_it), std::move(val));
    return key_it;
  }
  
  // BUG: Ambiguity with the upper methods
  // template <bool COND = !std::is_convertible_v<value_type, size_type>,
  //   typename = std::enable_if_t<COND>>
  // key_iterator insert(key_iterator key_it, size_type n, const_reference val = {}) {
  key_iterator insert(key_iterator key_it, size_type n, const value_type& val = value_type()) {
    // _vec.insert(_vec.begin() + key, n, {}); // BUG:
    _vec.insert(_vec.begin() + difference_type(*key_it), n, val);
    return key_it;
  }


  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  key_iterator insert(key_iterator key_it, IterType first, IterType last) {
    // static_assert(std::is_constructible_v<value_type, typename std::iterator_traits<IterType>::value_type>);
    // static_assert(std::is_convertible_v<value_type, typename std::iterator_traits<IterType>::value_type>);

    // _vec.insert(_vec.begin() + *key_it, first, last);
    _vec.insert(_vec.begin() + difference_type(*key_it), first, last);
    return key_it;
  }

  // BUG: The problem is that here ValType should be a row type
  // by default, but you give it vert_keys
  /*template <typename ValType, typename = std::enable_if_t<std::is_convertible_v<ValType, value_type>>>
  key_iterator insert(key_iterator key_it, std::initializer_list<ValType> l) {
    // _vec.insert(_vec.begin() + *key_it, l);
    _vec.insert(_vec.begin() + difference_type(*key_it), l);
    return key_it;
  }*/


  key_iterator erase(key_iterator key_it) {
    // _vec.erase(_vec.begin() + *key_it);
    _vec.erase(_vec.begin() + difference_type(*key_it));
    return key_it;
  }
  
  key_iterator erase(key_iterator first, key_iterator last) {
    _vec.erase(
      // _vec.begin() + *first,
      // _vec.begin() + *last
      _vec.begin() + difference_type(*first),
      _vec.begin() + difference_type(*last)
    );
    return first;
  }


  reference operator[](key_type key) {
    return _vec[key];
    // return _vec[size_type(key)];
  }

  const_reference operator[](key_type key) const {
    return _vec[key];
    // return _vec[size_type(key)];
  }
  
  reference at(key_type key) {
    return _vec.at(key);
    // return _vec.at(size_type(key));
  }

  const_reference at(key_type key) const {
    return _vec.at(key);
    // return _vec.at(size_type(key));
  }


};

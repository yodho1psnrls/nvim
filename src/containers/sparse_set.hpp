#pragma once

// #include "../meta_func.hpp"
#include "../iterators/index_iterator.hpp"

#include <iostream>

#include <iterator>
#include <type_traits>
#include <numeric>
#include <stdexcept>
#include <vector>

// NOTE: References:
// https://skypjack.github.io/2019-03-07-ecs-baf-part-2/
// https://skypjack.github.io/2020-08-02-ecs-baf-part-9/
// https://www.codeproject.com/Articles/859324/Fast-Implementations-of-Sparse-Sets-in-Cplusplus


// Mimics the interface of std::set<size_t>
class SparseSet {

  // using index_container = std::vector<size_t>;
  using index_container = std::vector<long long>;

public:

  /*using size_type = size_t;
  using difference_type = std::ptrdiff_t;

  using value_type = difference_type;
  using reference = value_type&;
  using const_reference = const difference_type&;
  using rvalue_reference = difference_type&&;
  using pointer = difference_type*;
  using const_pointer = const difference_type*;

  using iterator = typename std::vector<size_type>::iterator;
  using const_iterator = typename std::vector<size_type>::const_iterator;*/

  using size_type = typename index_container::size_type;
  using difference_type = typename index_container::difference_type;

  using key_type = typename index_container::value_type;
  using value_type = typename index_container::value_type;
  using const_reference = typename index_container::const_reference;
  using reference = const_reference;
  using const_pointer = typename index_container::const_pointer;
  using pointer = const_pointer;

  using const_iterator = typename index_container::const_iterator;
  using iterator = const_iterator;
  using const_reverse_iterator = typename index_container::const_reverse_iterator;
  using reverse_iterator = const_reverse_iterator;

  // static constexpr size_type INVALID_INDEX = -1;
  // static constexpr size_type INVALID_INDEX = std::numeric_limits<size_type>::max();
  static constexpr long long INVALID_INDEX = -1;

  // ------------------------------------------------------------------- //

  SparseSet() {}

  SparseSet(size_type size)
  : _dense(IndexIterator(0ULL), IndexIterator(size))
  , _sparse(IndexIterator(0ULL), IndexIterator(size)) {}


  bool empty() const {
    return _dense.empty();
  }

  size_type size() const {
    return _dense.size();
  }

  size_type max_size() const {
    return _sparse.size();
  }

  void clear() {
    _dense.clear();
    _sparse.clear();
  }

  bool contains(key_type val) const {
    // return _dense[_sparse[val]] == val;
    if (val < _sparse.size())
      return _sparse[val] != INVALID_INDEX;
    return false;
  }

  const_iterator find(key_type val) const {
    return _dense.cbegin() + _sparse[val];
  }

  const_reference operator[](key_type i) const {
    return _sparse[i];
  }

  const_reference at(key_type i) const {
    return _sparse[i];
  }


  void insert(key_type key) {
    if (contains(key))
      throw std::invalid_argument("Trying to insert an element into an already used position in the SparseSet");

    size_type n = _dense.size();
    _dense.push_back(key);
    if (key >= _sparse.size())
      _sparse.resize(key + 1, INVALID_INDEX);
    _sparse[key] = n;
  }

  void erase(key_type key) {
    if (!contains(key))
      throw std::invalid_argument("Trying to erase non-contained element in the SparseSet");

    // _sparse[_dense.back()] = _sparse[key];
    _dense[_sparse[key]] = _dense.back();
    _dense.pop_back();
    _sparse[key] = INVALID_INDEX;
    // --_sparse[_dense.size()];
  }

  void erase(const_iterator where) {
    erase(*where);
  }


  const_iterator begin() const { return _dense.begin(); }
  const_iterator end() const { return _dense.end(); }
  const_iterator cbegin() const { return _dense.cbegin(); }
  const_iterator cend() const { return _dense.cend(); }
  
  const_reverse_iterator rbegin() const { return _dense.rbegin(); }
  const_reverse_iterator rend() const { return _dense.rend(); }
  const_reverse_iterator crbegin() const { return _dense.crbegin(); }
  const_reverse_iterator crend() const { return _dense.crend(); }

  // DEBUGGING:

  bool is_correct() const {
    for(size_type i : _dense)
      if (_dense[_sparse[i]] != i)
        return false;
    return true;
  }

  void info() const {
    std::cout << "sparse: ";
    for(auto x : _sparse)
      std::cout << x << ", ";
    std::cout << "\n";

    std::cout << "dense: ";
    for(auto x : _dense)
      std::cout << x << ", ";
    std::cout << "\n";
  }

private:

  // NOTE:
  // The dense array on insert and erase acts the same as
  // the UnorderedVector wrapper class, and the sparse array
  // only follows the proper update of the mapping of the indexes
  // The expression _dense[_sparse[i]] == i always holds true !

  index_container _dense;
  index_container _sparse;

};


// ================================================================ //


template <typename T>
class SparseMap : public SparseSet {
public:

  SparseMap() {}

  SparseMap(size_t size)
  : SparseSet(size) {}
  
  SparseMap(const std::vector<T>& data)
  : SparseSet(data.size()), _data(data) {}

private:

  std::vector<T> _data;

};




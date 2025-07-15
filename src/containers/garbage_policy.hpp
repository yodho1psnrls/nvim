#pragma once

#include "../key_cont/adaptors/keys.hpp"
#include "../meta_func.hpp"

#include <type_traits>
#include <vector>


// Make the garbage policy, such that the user can 
//  choose a container from the bigger SoA container
//  that if it has a certain value, it means that
//  the element is removed.
// And if the user doesnt provide one, then the
//  garbage policy creates a separate boolean container
//  for checking if an element is removed
// So if you have a Basic Mesh class, you will have
//  those booleans that indicate if a vertex is removed
//  but if it is a HalfEdge Mesyh, the vertices will store
//  half-edge handlers, which can have a default value for
//  indicating if a vertex is removed, and doesnt need to
//  also contain another dangling container of booleans

// NOTE: We DONT need garbage policy for
// non vectory types

// CRTP: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/

// NOTE: Remember !!!
// The garbage_policy is good for keeping some insertions or
//  erasures const time, but it is more important that a good
//  garbage collection system shuffles the keys as least as possible


// Because its storing indexes, we can use bitmask
//  for garbaged elements
template <typename BaseType, typename KeyType>
class MaskGarbagePolicy {

  static_assert(std::is_same_v<
    typename handle_traits<KeyType>::handle_category,
    index_handle_tag>
  );

  BaseType& self() { return static_cast<BaseType&>(*this); }
  const BaseType& self() const { return static_cast<const BaseType&>(*this); }

  // For Safety
  MaskGarbagePolicy() {};
  MaskGarbagePolicy(size_t n) : _removed_mask(n, false) {};
  friend BaseType;

  // using size_type = size_t;
  using size_type = unsigned;

  std::vector<bool> _removed_mask;
  size_type _removed_count{};

public:

  bool are_shrinked() const {
    return !_removed_count;
  }
  
  size_type removed() const {
    return _removed_count;
  }

  KeyType add() {
    if (_removed_count) {
      --_removed_count;
      for (KeyType key : self().template range<KeyType>())
        if (_removed_mask[key])
          return key;
    }
    _removed_mask.push_back(false);
    return self().add();
  }

  void remove(KeyType key) {
    _removed_mask[key] = true;
    ++_removed_count;
  }

  void shrink() {
    for (KeyType key : reversed(self().template range<KeyType>()))
      if (_removed_mask[key])
        self().remove(key);
    _removed_mask.assign(_removed_mask.size(), false);
    _removed_count = 0;
  }

};


template <typename BaseType, typename KeyType>
class SetGarbagePolicy {

  BaseType& self() { return static_cast<BaseType&>(*this); }
  const BaseType& self() const { return static_cast<const BaseType&>(*this); }

  // For Safety
  SetGarbagePolicy() {};
  SetGarbagePolicy(size_t n) : _removed_mask(n, false) {};
  friend BaseType;

  // using size_type = size_t;
  using size_type = unsigned;

  std::vector<bool> _removed_mask;
  size_type _removed_count{};

public:

  bool is_shrinked() const {
    return !_removed_count;
  }
  
  size_type removed() const {
    return _removed_count;
  }

  KeyType add() {
    if (_removed_count) {
      --_removed_count;
      for (KeyType key : self().template range<KeyType>())
        if (_removed_mask[key])
          return key;
    }
    _removed_mask.push_back(false);
    return self().add();
  }

  void remove(KeyType key) {
    _removed_mask[key] = true;
    ++_removed_count;
  }

  void shrink() {
    for (KeyType key : reversed(self().template range<KeyType>()))
      if (_removed_mask[key]) {
        self().remove(key);
        _removed_mask[key] = false;
      }

    _removed_count = 0;
  }

};


// template <typename Policy>
// struct GarbagePolicyVert : private Policy {
//
//   using add_vert = Policy::add;
//
// };



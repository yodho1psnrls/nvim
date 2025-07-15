#pragma once

#include "index_iterator.hpp"
#include "transform_iterator.hpp"
// #include "zip_iterator.hpp"
// #include "crtp_iterator.hpp"
// #include <memory>


/*template <typename T, typename difference_type = std::ptrdiff_t>
struct DefaultAssignIterator
: public index_iterator<difference_type> {

  static_assert(std::is_default_constructible_v<T>);

  using base_type = index_iterator<difference_type>;
  using base_type::base_type;

  const T& value() const { return T(); }
  const base_type& base() const { return static_cast<const base_type&>(*this); }

  using value_type = T;
  using reference = T;
  using pointer = std::unique_ptr<T>;

  reference operator*() const { return T(); }
  pointer operator->() const { return std::make_unique<T>(); }
  reference operator[](difference_type i) const { return T(); }

};*/


/*template <typename DiffType, typename T>
// template <typename T, typename difference_type = int>
struct ValueAssignIterator
// : public index_iterator<difference_type> {
: public CrtpIterator<ValueAssignIterator<DiffType, T>, 

  // static_assert(std::is_copy_constructible_v<T>);
  static_assert(std::is_copy_assignable_v<T> || std::is_move_assignable_v<T>);
  using difference_type = DiffType;

  using base_type = index_iterator<difference_type>;
  using base_type::base_type;

  explicit ValueAssignIterator(difference_type id, const T& val = T())
  : base_type(id), _val(val) {}
  
  explicit ValueAssignIterator(index_iterator<difference_type> it, const T& val = T())
  : base_type(it), _val(val) {}
  
  const T& value() const { return T(); }
  const base_type& base() const { return static_cast<const base_type&>(*this); }

  using value_type = T;
  using reference = const T&;
  using pointer = const T*;
  // using pointer = std::unique_ptr<T>;

  reference operator*() const { return _val; }
  pointer operator->() const { return &_val; }
  // pointer operator->() const { return std::make_unique<T>(_val); }
  reference operator[](difference_type i) const { return _val; }

private:

  // T _val;
  const T& _val;

};


template <typename difference_type, typename T>
// template <typename T, typename difference_type>
ValueAssignIterator(difference_type, const T&)
-> ValueAssignIterator<difference_type, T>;
// -> ValueAssignIterator<T, difference_type>;*/


struct NoOpValue {
  template <typename T>
  NoOpValue& operator=(const T&) {
    return *this;
  }
  // template <typename T>
  // friend NoOpValue operator=(T&, NoOpValue) {
  //
  // }
};


template <typename ValType>
struct DefaultTypeUnaryFunctor {
  
  template <typename T>
  ValType operator()(const T&) const {
    return ValType();
  }

};

template <typename ValType, typename DiffType = std::ptrdiff_t>
using DefaultAssignIterator = TransformIterator<
  IndexIterator<DiffType>,
  DefaultTypeUnaryFunctor<ValType>
>;

template <typename DiffType = std::ptrdiff_t>
using NoOpIterator = DefaultAssignIterator<NoOpValue, DiffType>;


template <typename ValType>
struct ValueTypeUnaryFunctor {

  using value_type = std::remove_cvref_t<ValType>;
  using reference = const value_type&;

  ValueTypeUnaryFunctor(reference val)
  : _val(val) {}

  template <typename T>
  reference operator()(const T&) const {
    return _val;
  }

private:
  ValType _val;
};

template <typename ValType, typename DiffType = std::ptrdiff_t>
using ValueAssignIterator = TransformIterator<
  IndexIterator<DiffType>,
  ValueTypeUnaryFunctor<ValType>
>;

template <typename ValType, typename DiffType = std::ptrdiff_t>
using RefAssignIterator = ValueAssignIterator<const ValType&, DiffType>;


template <typename ValType, ValType Value>
struct StaticTypeUnaryFunctor {

  using value_type = std::remove_cvref_t<ValType>;
  using reference = const value_type&;

  template <typename T>
  reference operator()(const T&) const {
    return Value;
  }

};

template <typename ValType, ValType Value, typename DiffType = std::ptrdiff_t>
using StaticAssignIterator = TransformIterator<
  IndexIterator<DiffType>,
  StaticTypeUnaryFunctor<ValType, Value>
>;


template <typename ValueType>
struct KeyToPair {
  template <typename KeyType>
  std::pair<KeyType, ValueType> operator()(const KeyType& key) const {
    return {key, ValueType()};
  }
};

// Given an iterator of keys, it is an iterator that returns
template <typename KeyIter, typename ValType>
using MapDefaultAssignIterator = TransformIterator<KeyIter, KeyToPair<ValType>>;


// TODO:
/*template <typename ValueType>
struct KeySameValToPair {

  template <typename KeyType>
  std::pair<KeyType, ValueType> operator()(const KeyType& key) const {
    return {key, _val};
  }

  KeySameValToPair(const ValueType& val)
  : _val(val) {}

private:

  ValueType _val;

};

template <typename KeyIter, typename ValType>
using MapValueAssignIterator = TransformIterator<KeyIter, KeySameValToPair<ValType>>;

// template <typename KeyIter, typename ValType>
// using MapValueAssignIterator = zip_iterator<KeyIter, ValueAssignIterator;
*/




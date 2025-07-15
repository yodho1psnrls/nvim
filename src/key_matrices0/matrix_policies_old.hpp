#pragma once

#include<iostream>
#include <vector>
#include <cassert>
#include <stdexcept>

#include "../iterators/iterable.hpp"

#include "../iterators/jump_iterator.hpp"
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/flat_iterator.hpp"

#include "../meta_func.hpp"


#ifndef uint
typedef unsigned int uint;
#endif


// template <typename T = std::ptrdiff_t>
template <typename FlatCont>
struct FixedRowSizePolicy {

  using difference_type = typename FlatCont::difference_type;
  using size_type = typename FlatCont::size_type;

  FixedRowSizePolicy() : _n() {}
  FixedRowSizePolicy(difference_type n) : _n(n) {}
  
  using flat_value_type = typename FlatCont::value_type;
  using flat_iterator = typename FlatCont::iterator;
  using flat_const_iterator = typename FlatCont::const_iterator;

  using value_type = FlatCont;
  using reference = TensorIterable<flat_iterator, NoPolicy<>, FixedJumpPolicy<difference_type>>;
  using const_reference = TensorIterable<flat_const_iterator, NoPolicy<>, FixedJumpPolicy<difference_type>>;
  using iterator = tensor_iterator_layer<reference, UseItbSize<>>;
  using const_iterator = tensor_iterator_layer<const_reference, UseItbSize<>>;

  
  // iterator begin() { return iterator(reference(_vec.begin(), NoPolicy(), _n), {}); }
  // iterator end() { return iterator(reference(_vec.end(), NoPolicy(), _n), {}); }
  // const_iterator begin() const { return const_iterator(const_reference(_vec.begin(), NoPolicy()), {}); }
  // const_iterator end() const { return const_iterator(const_reference(_vec.end(), NoPolicy(), _n), {}); }
  // const_iterator cbegin() const { return const_iterator(const_reference(_vec.cbegin(), NoPolicy(), _n), {}); }
  // const_iterator cend() const { return const_iterator(const_reference(_vec.cend(), NoPolicy(), _n), {}); }

  iterator begin(FlatCont& vec) { return iterator(reference(vec.begin(), NoPolicy(), _n), {}); }
  iterator end(FlatCont& vec) { return iterator(reference(vec.end(), NoPolicy(), _n), {}); }
  const_iterator begin(const FlatCont& vec) const { return iterator(reference(vec.begin(), NoPolicy(), _n), {}); }
  const_iterator end(const FlatCont& vec) const { return iterator(reference(vec.end(), NoPolicy(), _n), {}); }
  const_iterator cbegin(const FlatCont& vec) const { return iterator(reference(vec.cbegin(), NoPolicy(), _n), {}); }
  const_iterator cend(const FlatCont& vec) const { return iterator(reference(vec.cend(), NoPolicy(), _n), {}); }


  const_iterator insert(FlatCont& vec, iterator where, const value_type& new_row) {
    // if constexpr (DYNAMIC_ROW_SIZE)
    //   //if (_n == ZERO_INDEX)
    //   if(_vec.empty())
    //     _n = new_row.size();

    if(!new_row.size())
      return {vec.insert(where->begin(), _n, {}), NoPolicy(), _n, UseItbSize()};

    if (new_row.size() != _n)
      throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");
    
    //return {vec.insert(where.base(), new_row.begin(), new_row.end()), _n};
    return {vec.insert(where->begin(), new_row.begin(), new_row.end()), NoPolicy(), _n, UseItbSize()};
  }
  
  template <typename IterType>
  iterator insert(FlatCont& vec, const const_iterator& where, const IterType& first_row_it, const IterType& last_row_it) {
    // if constexpr (DYNAMIC_ROW_SIZE)
    //   //if (_n == ZERO_INDEX)
    //   if(vec.empty())
    //     _n = first_row_it->size();

    for(auto it = first_row_it; it != last_row_it; ++it)
      if (it->size() != _n)
        throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    __flat_iterator<IterType> flat_first(first_row_it);
    __flat_iterator<IterType> flat_last(last_row_it);

    //return {vec.insert(where.base(), flat_first, flat_last), _n};
    return {vec.insert(where->begin(), flat_first, flat_last), NoPolicy(), _n, UseItbSize()};
  }

  /*template <typename OtherMatrix = std::initializer_list<std::initializer_list<value_type>>>
  iterator insert_range(FlatCont& vec, const const_iterator& where, const OtherMatrix& new_rows) {
    // if constexpr (DYNAMIC_ROW_SIZE)
    //   //if (_n == ZERO_INDEX)
    //   if(vec.empty())
    //     _n = new_rows.begin()->size();

    for(auto row : new_rows)
      if (row.size() != _n)
        throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    __flat_iterator flat_first(new_rows.begin());
    __flat_iterator flat_last(new_rows.end());

    return {vec.insert(where->begin(), flat_first, flat_last), NoPolicy(), _n, UseItbSize()};
  }*/


  iterator erase(FlatCont& vec, const const_iterator& where) {
    // if (vec.size() == _n) // If only one row
    //   _n = ZERO_INDEX;

    //return vec.erase(where.base(), where.base() + _n);
    return {vec.erase(where->begin(), where->end()), NoPolicy(), _n, UseItbSize()};
  }

  iterator erase(FlatCont& vec, const const_iterator& first_row_it, const const_iterator& last_row_it) {
    // if (vec.size() == std::distance(first_row_it->begin(), last_row_it->begin()))
    //   _n = ZERO_INDEX;

    //return vec.erase(first_row_it.base(), last_row_it.base());
    return {vec.erase(first_row_it->begin(), last_row_it->begin()), NoPolicy(), _n, UseItbSize()};
  }


private:

  difference_type _n;

};


template <typename IndexType>
struct JaggedRowByRangeOfIndices {
  
  template <typename Container = std::initializer_list<T>>
  iterator insert(const const_iterator& where, const Container& new_row) {
  // void insert(const const_iterator& where, const Container& new_row) {
    int n = new_row.size();
    auto val_it = _vec.insert(where->begin(), new_row.begin(), new_row.end());

    auto id_it = _row_ranges.insert(where.policy().base() + 1, *(where.policy().base()));
    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it += n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }


  template <typename IterType>
  iterator insert(const const_iterator& where, const IterType& first_row_it, const IterType& last_row_it) {
    __flat_iterator<IterType> flat_first(first_row_it);
    __flat_iterator<IterType> flat_last(last_row_it);
    
    auto val_it = _vec.insert(where->begin(), flat_first, flat_last);

    // NOTE: Here you can use boost::transform iterator
    // that transforms the given rows to row.size() + *where->policy().base()
    auto id_it = _row_ranges.insert(where.policy().base() + 1, std::distance(first_row_it, last_row_it), *(where.policy().base()));

    auto iit = id_it;
    difference_type n = 0;
    for(auto it = first_row_it; it != last_row_it; ++it) {
      n += it->size();
      *(iit++) += n;
    }

    while(iit != _row_ranges.end())
      *(iit++) += n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }


  iterator erase(const const_iterator& where) {
    int n = where->size();
    auto val_it = _vec.erase(where->begin(), where->end());
    auto id_it = _row_ranges.erase(where.policy().base() + 1);
    
    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }

  iterator erase(const const_iterator& first_row_it, const const_iterator& last_row_it) {
    // int n = *(last_row_it.policy().base()) - *(first_row_it.policy().base());
    int n = last_row_it.base() - first_row_it.base();
    
    auto val_it = _vec.erase(first_row_it.base(), last_row_it.base());
    auto id_it = _row_ranges.erase(first_row_it.policy().base() + 1, last_row_it.policy().base() + 1);

    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;
    
    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }

  // ------------------------- In Row insert/erase methods ----------------- //


  flat_iterator insert(
    const const_iterator& row_it,
    const flat_const_iterator& val_it,
    const T& val
  ) {
    assert(row_it->begin() <= val_it && val_it <= row_it->end());
    // auto it = row_it.policy().base();
    auto it = _row_ranges.erase(row_it.policy().base(), row_it.policy().base());
    ++it;

    for(; it != _row_ranges.end(); ++it)
      ++(*it);

    return _vec.insert(val_it.base(), val);
  }


  template <typename IterType>
  flat_iterator insert(
    const const_iterator& row_it,
    const flat_const_iterator& val_it,
    const IterType& first,
    const IterType& last
  ) {
    assert(row_it->begin() <= val_it && val_it <= row_it->end());
    // auto it = row_it.policy().base(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().base(), row_it.policy().base());
    ++it;

    auto n = std::distance(first, last);
    for(; it != _row_ranges.end(); ++it)
      *it += n;

    return _vec.insert(val_it.base(), first, last);
  }


  flat_iterator erase(
    const const_iterator& row_it,
    const flat_const_iterator& val_it
  ) {
    assert(row_it->begin() <= val_it && val_it < row_it->end());
    // auto it = row_it.policy().base(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().base(), row_it.policy().base());
    ++it;

    for(; it != _row_ranges.end(); ++it)
      --(*it);

    return _vec.erase(val_it.base());
  }


  flat_iterator erase(
    const const_iterator& row_it,
    const flat_const_iterator& from,
    const flat_const_iterator& to
  ) {
    assert(row_it->begin() <= from && from < row_it->end());
    assert(row_it->begin() <= to && to < row_it->end());
    // auto it = row_it.policy().base(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().base(), row_it.policy().base());
    ++it;

    auto n = std::distance(from, to);
    for(; it != _row_ranges.end(); ++it)
      *it -= n;

    return _vec.erase(from.base(), to.base());
  }
  
};

// Requires a jump policy that steps to the
// next iterator, instead of incrementing
// This matrix can work with std::list
template <typename IndexType>
struct JaggedRowByRangeOfIterators {
  
};


template <typename DifferenceType>
struct JaggedRowByJumpCirculator {

};





// TODO: Template this with BaseVectorType, so you can choose between
//  different container types that support the std::vector interface

// https://stackoverflow.com/questions/42668335/is-it-possible-to-have-a-matrix-with-different-row-sizes-in-c
// https://www.geeksforgeeks.org/jagged-arrays-in-cpp/
// https://stackoverflow.com/questions/18269123/ragged-and-jagged-arrays
// A Jagged/Ragged Array/Matrix Implementation
template <typename VecType, typename Policy>
class Matrix {
  
  static_assert(is_container_v<VecType>);

  // static const uint INVALID_INDEX = static_cast<uint>(-1);
  using values_vec_type = VecType;

  using ranges_vec_type = std::vector<
    typename values_vec_type::iterator::difference_type>;

  using T = typename VecType::value_type;

public:

  using value_type = values_vec_type;
 
  using reference = TensorIterable<typename values_vec_type::iterator, NoPolicy<>, JumpByIndexDiff<typename ranges_vec_type::const_iterator>>;
  using const_reference = TensorIterable<typename values_vec_type::const_iterator, NoPolicy<>, JumpByIndexDiff<typename ranges_vec_type::const_iterator>>;
  // using reverse_reference = TensorIterable<typename values_vec_type::reverse_iterator, NoPolicy<>, JumpByIndexDiff<typename ranges_vec_type::const_reverse_iterator>>;
  // using const_reverse_reference = TensorIterable<typename values_vec_type::const_reverse_iterator, NoPolicy<>, JumpByIndexDiff<typename ranges_vec_type::const_reverse_iterator>>;
  
  using pointer = reference*;
  using const_pointer = const_reference*;
  // using reverse_pointer = reverse_reference*;
  // using const_reverse_pointer = const_reverse_reference*;


  using allocator_type = typename VecType::allocator_type;
  using size_type = uint;
  using difference_type = std::ptrdiff_t;

  using iterator = tensor_iterator_layer<reference, UseItbSize<>>;
  using const_iterator = tensor_iterator_layer<const_reference, UseItbSize<>>;
  // using reverse_iterator = tensor_iterator_layer<reverse_reference, UseItbSize<>>;
  // using const_reverse_iterator = tensor_iterator_layer<const_reverse_reference, UseItbSize<>>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using flat_value_type = T;
  using flat_reference = typename values_vec_type::reference;
  using flat_const_reference = typename values_vec_type::const_reference;
  using flat_iterator = jump_iterator<typename values_vec_type::iterator, NoPolicy<>>;
  using flat_const_iterator = jump_iterator<typename values_vec_type::const_iterator, NoPolicy<>>;


  const values_vec_type& flat() const { return _vec; }
  values_vec_type& flat() { return _vec; }

  // Iterable<flat_const_iterator> flat() const {
  //   return Iterable(
  //     _vec.cbegin(),
  //     _vec.cend()
  //   );
  // }
  
  // Iterable<flat_iterator> flat() {
  //   return Iterable(
  //     _vec.begin(),
  //     _vec.end()
  //   );
  // }


  // --------------------------- Capacity ------------------------------ //

  bool empty() const { return _vec.empty(); }
  // bool empty() const { return _row_ranges.size() == 1; }

  size_type size() const { return _row_ranges.size() - 1; }

  // ------------------------- Constructors ---------------------------- //

  Matrix() : _vec(), _row_ranges({0}) {}

  template <typename IterType>
  Matrix(const IterType& first, const IterType& last) : Matrix() {
    insert(end(), first, last);
  }

  template <typename Container = std::initializer_list<T>>
  Matrix(size_type n, const Container& val = {}) : Matrix() {
    static_assert(is_container_v<Container>);
    reserve(n);
    reserve_flat(n * val.size());
    while(n--)
      push_back(val);
  }

  // template <typename Container = std::initializer_list<std::initializer_list<T>>>
  // JaggedMatrix()

  // -------------------------- Modifiers ------------------------------ //

  void clear() {
    _vec.clear();
    _row_ranges.assign({0});
  }

  void reserve(size_type num_rows) {
    _row_ranges.reserve(1 + num_rows);
  }
  
  void reserve_flat(size_type num_val) {
    _vec.reserve(num_val);
  }
  
  // TODO: Fix resize
  // void resize(size_type num_rows) {
  //   _row_ranges.resize(1 + num_rows);
  // }
  // 
  // void resize_flat(size_type num_val) {
  //   _vec.resize(num_val);
  // }

  template <typename Container = std::initializer_list<T>>
  void push_back(const Container &new_row) {
    //_row_ranges.push_back({ static_cast<uint>(_vec.size()), static_cast<uint>(new_row.size()) });

    _vec.insert(_vec.end(), new_row.begin(), new_row.end());
    _row_ranges.push_back(_vec.size());
  }

  void pop_back() {
    _vec.erase(
      _vec.begin() + *(_row_ranges.end() - 2),
      _vec.end()
    );

    _row_ranges.pop_back();

    //_vec.erase(_vec.end() - _n - 1, _vec.end() - 1);
  }

  // IMPLEMENT INSERT AND ERASE HERE //

  // WE WILL MAKE IT CONSISTENT WITH STD::VECTOR INSERT AND ERASE SHIFT LOGIC




  // -------------------------- Element Access ----------------------------- //

  const_reference operator[](difference_type id) const {
    return *(cbegin() + id);
  }

  reference operator[](difference_type id) {
    return *(begin() + id);
  }

  const_reference at(difference_type id) const {
    if (0 < id || id >= _row_ranges.size())
      throw std::out_of_range("Out of range at() method");
    return *(cbegin() + id);
  }

  reference at(difference_type id) {
    if (id >= _row_ranges.size())
      throw std::out_of_range("Out of range at() method");
    return *(begin() + id);
  }

  const_reference operator()(difference_type id) const {
    return *(cbegin() + id);
  }

  reference operator()(difference_type id) {
    return *(begin() + id);
  }

  flat_const_reference operator()(size_type row_id, difference_type val_id) const {
    return _vec[_row_ranges[row_id] + val_id];
  }

  flat_reference operator()(size_type row_id, difference_type val_id) {
    return _vec[_row_ranges[row_id] + val_id];
  }
  
  flat_const_reference at(size_type row_id, difference_type val_id) const {
    return _vec[_row_ranges[row_id] + val_id];
  }

  flat_reference at(size_type row_id, difference_type val_id) {
    return _vec[_row_ranges[row_id] + val_id];
  }



  // --------------------------- Iterators ------------------------------ //

  iterator begin() { return iterator(reference(_vec.begin(), NoPolicy(), _row_ranges.cbegin()), {}); }
  // iterator begin() { return iterator(_vec.begin(), NoPolicy(), _row_ranges.cbegin(), UseItbSize()); }
  iterator end() { return iterator(reference(_vec.end(), NoPolicy(), _row_ranges.cend() - 1), {}); }
  const_iterator begin() const { return const_iterator(const_reference(_vec.cbegin(), NoPolicy(), _row_ranges.cbegin()), {}); }
  const_iterator end() const { return const_iterator(const_reference(_vec.cend(), NoPolicy(), _row_ranges.cend() - 1), {}); }
  const_iterator cbegin() const { return const_iterator(const_reference(_vec.cbegin(), NoPolicy(), _row_ranges.cbegin()), {}); }
  const_iterator cend() const { return const_iterator(const_reference(_vec.cend(), NoPolicy(), _row_ranges.cend() - 1), {}); }

  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator rend() const { return std::reverse_iterator(cbegin()); }
  const_reverse_iterator rcbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator rcend() const { return std::reverse_iterator(cbegin()); }

  // reverse_iterator rbegin() { return reverse_iterator(reverse_reference(_vec.rend(), NoPolicy(), _row_ranges.crbegin()), {}); }
  // reverse_iterator rend() { return reverse_iterator(reverse_reference(_vec.rbegin(), NoPolicy(), _row_ranges.crend() - 1), {}); }
  // const_reverse_iterator rbegin() const { return const_reverse_iterator(const_reverse_reference(_vec.crend(), NoPolicy(), _row_ranges.crbegin()), {}); }
  // const_reverse_iterator rend() const { return const_reverse_iterator(const_reverse_reference(_vec.crbegin(), NoPolicy(), _row_ranges.crend() - 1), {}); }
  // const_reverse_iterator rcbegin() const { return const_reverse_iterator(const_reverse_reference(_vec.crend(), NoPolicy(), _row_ranges.crbegin()), {}); }
  // const_reverse_iterator rcend() const { return const_reverse_iterator(const_reverse_reference(_vec.crbegin(), NoPolicy(), _row_ranges.crend() - 1), {}); }


  // ------------------------- Private Members ---------------------------- //

private:

  ranges_vec_type _row_ranges;
  values_vec_type _vec; // flat vector for all elements

};

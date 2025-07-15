#pragma once

#include <vector>
#include <cassert>
#include <stdexcept>
#include <ostream>

#include "../iterable.hpp"
#include "iterable_iterator.hpp"
#include "jump_iterator.hpp"

#include "../flat_iterator.hpp"

#include <iostream>


// TODO: Implement properly one of those Allocators to be able to call
// _vec.resize(), without initializing the data, so you can store
// non-default constructible elements in the Matrix class

// This custom allocator allows us to resize std::vector, without constructing the new elements
// So if we can handle Non-Constructible value types, and resize at will
/*template <typename T>
class NoInitializeAllocator {
  // https://www.geeksforgeeks.org/resize-vector-without-initializing-new-elements-in-cpp/

  using value_type = T;
  
  // Allocate sufficient memory
  T* allocate(size_t n) {
      return static_cast<T*>(::operator new(n * sizeof(T)));
  }
  
  void deallocate(T* p, size_t n) noexcept {
      ::operator delete(p, sizeof(T) * n);
  }

  // Override construct to skip initialization
  template <typename U, typename... Args>
  void construct(U* p, Args&&...) noexcept {
    // No initialization occurs
  }

  template <typename U>
  void destroy(U* p) noexcept {
      p->~U();
  }

};*/


/*template <typename Alloc>
class NoInitializeAllocator : public Alloc {
//template <typename T, template <typename _T> class Alloc = std::allocator>
//class NoInitializeAllocator : public Alloc<T> {
  // https://www.geeksforgeeks.org/resize-vector-without-initializing-new-elements-in-cpp/
  
  // Override construct to skip initialization
  template <typename U, typename... Args>
  void construct(U* p, Args&&...) noexcept {
    // No initialization occurs
  }

};*/


// Remember, moving on a flat matrix is a 2D problem to 1D
//  by x is +=1, moving by y is += row_size
// Do it somehow, such that when you swap the two MoveRowIter and MoveColIter
//  policies, it converts between column and row iterators
/*template <typename BaseIter, typename MoveRowIter, typename MoveColIter>
class mat_iterator {
public:

  using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

  BaseIter _beg;
  difference_type _n_x;
  difference_type _n_y;
  
  //MoveRowIter::reference operator*() const { return }

};*/



// TODO:
// In the current implementation, the column iterators
// hold an iterable with two jump iterators and they both
// contain stride length _n, this is redundant. Make it such
// that the column iterator holds this stride length and the base
// (vector) iterators, then converting to Iterable of jump iterators
// at operator* and operator->, but for the pointer you may need to
// use std::unique_ptr


// A Row-Major Matrix-Like Container
// This is not a Mathematical matrix, this only
// provides base memory management for a matrix
template <typename T, typename Alloc = std::allocator<T>>
class Matrix {
public:
  using allocator_type = Alloc;
  using difference_type = std::ptrdiff_t;

private:

  static constexpr difference_type ZERO_INDEX = 0U;
  using vec_type = std::vector<T, Alloc>;
  //using vec_type = std::vector<T, NoInitializeAllocator<Alloc>>;


public:

  using size_type = typename vec_type::size_type;


  using row_type = vec_type;
 
  using row_reference = Iterable<typename vec_type::iterator>;
  using const_row_reference = InputIterable<typename vec_type::const_iterator>;
  using reverse_row_reference = Iterable<typename vec_type::reverse_iterator>;
  using const_reverse_row_reference = InputIterable<typename vec_type::const_reverse_iterator>;
  
  using row_pointer = row_reference*;
  using const_row_pointer = const_row_reference*;
  //using row_pointer = std::unique_ptr<row_reference>;
  //using const_row_pointer = std::unique_ptr<const_row_reference>;
  using reverse_row_pointer = reverse_row_reference*;
  using const_reverse_row_pointer = const_reverse_row_reference*;


  using col_type = vec_type;

  using col_reference = Iterable<fixed_jump_iterator<typename vec_type::iterator>>;
  using const_col_reference = InputIterable<fixed_jump_iterator<typename vec_type::const_iterator>>;
  using reverse_col_reference = Iterable<fixed_jump_iterator<typename vec_type::reverse_iterator>>;
  using const_reverse_col_reference = InputIterable<fixed_jump_iterator<typename vec_type::const_reverse_iterator>>;
  
  using col_pointer = row_reference*;
  using const_col_pointer = const_row_reference*;
  //using col_pointer = std::unique_ptr<row_reference>;
  //using const_col_pointer = std::unique_ptr<const_row_reference>;
  using reverse_col_pointer = reverse_col_reference*;
  using const_reverse_col_pointer = const_reverse_col_reference*;


  // ----------------------------------- Iterators ------------------------------------------ //

  using row_iterator = iterable_iterator<row_reference, IncrementIteratorsByIterableSize>;
  using const_row_iterator = iterable_iterator<const_row_reference, IncrementIteratorsByIterableSize>;
  using reverse_row_iterator = iterable_iterator<reverse_row_reference, IncrementIteratorsByIterableSize>;
  using const_reverse_row_iterator = iterable_iterator<const_reverse_row_reference, IncrementIteratorsByIterableSize>;

  using col_iterator = iterable_iterator<col_reference, IncrementIteratorsBaseByOne>;
  using const_col_iterator = iterable_iterator<const_col_reference, IncrementIteratorsBaseByOne>;
  using reverse_col_iterator = iterable_iterator<reverse_col_reference, IncrementIteratorsBaseByOne>;
  using const_reverse_col_iterator = iterable_iterator<const_reverse_col_reference, IncrementIteratorsBaseByOne>;


  // ---------------------- Generalization ------------------------- //


  using value_type = row_type;
  using reference = row_reference;
  using const_reference = const_row_reference;
  using pointer = row_pointer;
  using const_pointer = const_row_pointer;
  
  using iterator = row_iterator;
  using const_iterator = const_row_iterator;
  using reverse_iterator = reverse_row_iterator;
  using const_reverse_iterator = const_reverse_row_iterator;


private:


  template <typename ValueIter>
  col_iterator _insert_col(const const_col_iterator& where, const ValueIter& first_val_it, const ValueIter& last_val_it, difference_type values_size) {
    if(_n != ZERO_INDEX && values_size != rows_size())
      throw std::out_of_range("The the number of elements in the new column should be the same as the number of rows");
   
    // number of elements in a single column
    // this is dependant on _vec.size(), so its important NOT to resize _vec before this
   // difference_type col_size = rows_size();
    difference_type col_size = values_size;
    difference_type offset = (where->begin().base() - _vec.cbegin());
    difference_type i = _n - offset;

    _vec.resize(_vec.size() + col_size);

    for(auto rit = _vec.rbegin(); rit != _vec.rend(); ++rit) {
      if (i-- != 0)
        *rit = std::move(*(rit + col_size));
      else {
        i = _n;
        --col_size;
      }
    }
    
    auto it = _vec.begin() + offset;
    ++_n;
    
    for(auto val_it = first_val_it; val_it != last_val_it; ++val_it) {
      *it = *val_it;
      it += _n;
    }

    return col_reference(
      fixed_jump_iterator(_vec.begin() + offset, _n),
      fixed_jump_iterator(_vec.end() + offset, _n)
    );
  }


  template <typename IterType>
  col_iterator _insert_cols(const const_col_iterator& where, const IterType& first_col_it, const IterType& last_col_it, difference_type num_of_new_cols) {

    // number of elements in a single column
    // this is dependant on _vec.size(), so its important NOT to resize _vec before this
   // difference_type col_size = rows_size();
    difference_type col_size = first_col_it->size();
    difference_type offset = (where->begin().base() - _vec.cbegin());
    difference_type i = _n - offset;

    if (_n != ZERO_INDEX)
      for(auto it = first_col_it; it != last_col_it; ++it)
        if (it->size() != col_size)
          throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    col_size *= num_of_new_cols;

    // Moving and making space for the new elements

    _vec.resize(_vec.size() + col_size);
    
    for(auto rit = _vec.rbegin(); rit != _vec.rend();) {
      if (i-- != 0) {
        *rit = std::move(*(rit + col_size));
        ++rit;
      }
      else {
        i = _n;
        col_size -= num_of_new_cols;
        rit += num_of_new_cols;
      }
    }

    // Assigning the elements

    auto in_col_it = _vec.begin() + offset;
    auto it = in_col_it;
    _n += num_of_new_cols;
    
    for(auto new_col_it = first_col_it; new_col_it != last_col_it; ++new_col_it) {
      for(auto val : *new_col_it) {
        *it = val;
        it += _n;
      }
      ++in_col_it;
      it = in_col_it;
    }

    //std::cout << "offset: " << offset << "\n";

    return col_reference(
      fixed_jump_iterator(_vec.begin() + offset, _n),
      fixed_jump_iterator(_vec.end() + offset, _n)
    );
  }

  template <typename OtherValue, typename OtherAlloc>
  friend class Matrix;
  

public:


  // --------------------------- Capacity ------------------------------ //

  bool empty() const { return _vec.empty(); }

  size_type rows_size() const { return _n == 0U ? 0U : _vec.size() / _n; }
  size_type cols_size() const { return _n; }
  //size_type flat_size() const { return _vec.size(); }

  size_type size() const { return rows_size(); }

  // ------------------------- Constructors ---------------------------- //

  Matrix() : _n(ZERO_INDEX), _vec() {}

  //template <typename FlatContainer = vec_type>
  template <typename FlatContainer = std::initializer_list<T>>
  Matrix(difference_type size_of_row, const FlatContainer &flat_data)
    : _n(size_of_row), _vec(flat_data.begin(), flat_data.end()) {
    if (size_of_row < 1)
      throw std::invalid_argument("Row size cannot be smaller than 1");
    if (flat_data.size() % size_of_row != 0)
      throw std::invalid_argument("The size of the flat data container, should be divisible by size_of_row");
  }


  Matrix(const std::initializer_list<std::initializer_list<T>>& mat) 
    : _n(ZERO_INDEX) {
    insert_rows(end_rows(), mat);
  }

  template <typename OtherMatrix>
  Matrix(const OtherMatrix& mat)
    : _n(ZERO_INDEX) {
    insert_rows(end_rows(), mat);
  }

  template <typename OtherValue, typename OtherAlloc, typename = std::enable_if_t<std::is_constructible_v<T, OtherValue>>>
  Matrix(const Matrix<OtherValue, OtherAlloc>& other)
    : _vec(other._vec.begin(), other._vec.end()), _n(other._n) {}

  /*template <typename OtherValue, typename OtherAlloc>
  Matrix& operator=(const Matrix<OtherValue, OtherAlloc>& other) {
    _vec.assign(other._vec.begin(), other._vec.end());
    _n = other._n;
    return *this;
  }*/


  /*template <typename OtherMatrix = std::initializer_list<std::initializer_list<T>>>*/
  /*Matrix(const OtherMatrix& mat) */
  /*  : _n(ZERO_INDEX) {*/
  /*  insert_rows(end_rows(), mat);*/
  /*}*/


  //Matrix(difference_type size_of_row = ZERO_INDEX, const vec_type &vec = vec_type())
  //  : _n(size_of_row), _vec(vec) {}


  const vec_type& flat() const { return _vec; }
  vec_type& flat() { return _vec; }


  // -------------------------- Modifiers ------------------------------ //

  void clear() {
    _vec.clear();
    _n = ZERO_INDEX;
  }

  void reserve_rows(size_type num_of_rows) {
    if(_n == 0)
      throw std::out_of_range("Cannot reserve number of rows if the size of the rows is unknown");
    _vec.reserve(_n * num_of_rows);
  }
  
  void reserve_cols(size_type num_of_cols) {
    if(_n == 0)
      throw std::out_of_range("Cannot reserve number of columns if the size of the columns is unknown");
    _vec.reserve(rows_size() * num_of_cols);
  }

  //void reserve_flat(size_type num_of_values) {
  //  _vec.reserve(num_of_values);
  //}


  template <typename Container = std::initializer_list<T>>
  void push_row(const Container &new_row) {
    if (_n == ZERO_INDEX)
      _n = new_row.size();
    else if (new_row.size() != _n)
      throw std::out_of_range(
          "Trying to add a larger size row in a matrix of fixed row size");

    _vec.insert(_vec.end(), new_row.begin(), new_row.end());
  }


  template <typename Container = std::initializer_list<T>>
  void push_col(const Container &new_col) {
    if(_n != ZERO_INDEX && new_col.size() != rows_size())
      throw std::out_of_range("The the number of elements in the new column should be the same as the number of rows");
   
    // number of elements in a single column
    // this is dependant on _vec.size(), so its important NOT to resize _vec before this
    difference_type col_size = rows_size(); 
    difference_type i = 0;  // starting at 0 is push back
    //difference_type i = _n; // starting at _n is push front

    _vec.resize(_vec.size() + new_col.size());

    using other_iterator = Container::const_iterator;
    using IC = typename std::iterator_traits<typename Container::iterator>::iterator_category;
    constexpr bool is_bidir = std::is_base_of_v<std::bidirectional_iterator_tag, IC>;
    // If its bidirectional, then we can insert the values directly in reverse in the first loop
    // Else, we need a second loop to set the new elements

    for(auto rit = _vec.rbegin(); rit != _vec.rend(); ++rit) {
      if (i-- != 0)
        *rit = std::move(*(rit + col_size));
      else {
        i = _n;
        --col_size;
      }
    }
    
    auto it = _vec.begin() + _n;
    ++_n;
    
    for(const auto& val : new_col) {
      *it = val;
      it += _n;
    }

  }


  void pop_row() {
    _vec.erase(_vec.end() - _n - 1, _vec.end() - 1);
  }

  // TODO:
  /*void pop_col() {*/
  /*  difference_type offset = _n - 1;*/
  /*  for(auto it = _vec.begin() + offset; it != _vec.end() + offset; it += _n)*/
  /*    _vec.erase()*/
  /**/
  /*}*/


  // IMPLEMENT INSERT AND ERASE HERE //
  // WE WILL MAKE IT CONSISTENT WITH STD::VECTOR INSERT AND ERASE SHIFT LOGIC

  template <typename ValueIter>
  row_iterator insert_row(const const_row_iterator& where, const ValueIter& first_val_it, const ValueIter& last_val_it) {
    difference_type new_row_size = std::distance(first_val_it, last_val_it);
    //if (_n == ZERO_INDEX)
    if(_vec.empty())
      _n = new_row_size;
    else if (new_row_size != _n)
      throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");
    
    return { _vec.insert(where->begin(), first_val_it, last_val_it), _n };
  }

  template <typename Container = std::initializer_list<T>>
  row_iterator insert_row(const const_row_iterator& where, const Container& new_row) {
    //if (_n == ZERO_INDEX)
    if(_vec.empty())
      _n = new_row.size();
    else if (new_row.size() != _n)
      throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");
    
    //return {_vec.insert(where.base(), new_row.begin(), new_row.end()), _n};
    return {_vec.insert(where->begin(), new_row.begin(), new_row.end()), _n};
  }

  template <typename IterType>
  row_iterator insert_rows(const const_row_iterator& where, const IterType& first_row_it, const IterType& last_row_it) {
    //if (_n == ZERO_INDEX)
    if(_vec.empty())
      _n = first_row_it->size();
    else
      for(auto it = first_row_it; it != last_row_it; ++it)
        if (it->size() != _n)
          throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    __flat_iterator<IterType> flat_first(first_row_it);
    __flat_iterator<IterType> flat_last(last_row_it);

    //return {_vec.insert(where.base(), flat_first, flat_last), _n};
    return {_vec.insert(where->begin(), flat_first, flat_last), _n};
  }

  template <typename OtherMatrix = std::initializer_list<std::initializer_list<T>>>
  row_iterator insert_rows(const const_row_iterator& where, const OtherMatrix& new_rows) {
    //if (_n == ZERO_INDEX)
    if(_vec.empty())
      _n = new_rows.begin()->size();
    else
      for(auto row : new_rows)
        if (row.size() != _n)
          throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    __flat_iterator flat_first(new_rows.begin());
    __flat_iterator flat_last(new_rows.end());

    return {_vec.insert(where->begin(), flat_first, flat_last), _n};
  }


  template <typename ValueIter>
  col_iterator insert_col(const const_col_iterator& where, const ValueIter& first_val_it, const ValueIter& last_val_it) {
    return _insert_col(where, first_val_it, last_val_it, std::distance(first_val_it, last_val_it));
  }

  template <typename Container = std::initializer_list<T>>
  col_iterator insert_col(const const_col_iterator& where, const Container &new_col) {
    return _insert_col(where, new_col.begin(), new_col.end(), new_col.size());
  }


  template <typename IterType>
  col_iterator insert_cols(const const_col_iterator& where, const IterType& first_col_it, const IterType& last_col_it) {
    return _insert_cols(where, first_col_it, last_col_it, std::distance(first_col_it, last_col_it));
  }
  
  template <typename OtherMatrix = std::initializer_list<std::initializer_list<T>>>
  col_iterator insert_cols(const const_col_iterator& where, const OtherMatrix &new_cols) {
    return _insert_cols(where, new_cols.begin(), new_cols.end(), new_cols.size());
  }



  row_iterator erase_row(const const_row_iterator& where) {
    if (_vec.size() == _n) // If only one row
      _n = ZERO_INDEX;

    //return _vec.erase(where.base(), where.base() + _n);
    return {_vec.erase(where->begin(), where->end()), _n};
  }

  row_iterator erase_rows(const const_row_iterator& first_row_it, const const_row_iterator& last_row_it) {
    // If the current number of elements is the same as the elements that are gonna be erased
    //if (_vec.size() == _n * std::distance(first_row_it, last_row_it))
    if (_vec.size() == std::distance(first_row_it->begin(), last_row_it->begin()))
      _n = ZERO_INDEX;

    //return _vec.erase(first_row_it.base(), last_row_it.base());
    return {_vec.erase(first_row_it->begin(), last_row_it->begin()), _n};
  }


  col_iterator erase_col(const const_col_iterator& where) {
    --_n;
    difference_type offset = (where->begin().base() - _vec.cbegin());
    difference_type i = _n - offset - 1;
    
    auto it = _vec.begin();
    auto oit = _vec.begin();

    //while(oit != _vec.end() && oit != _vec.end() + 1) {
    while(oit < _vec.end()) {
      if(++i == _n) {
        i = 0;
        ++oit;
      }
      *it = std::move(*oit);
      ++it;
      ++oit;
    }

    //_vec.erase(_vec.end() - rows_size(), _vec.end());
    _vec.erase(_vec.end() - _vec.size() / (_n + 1), _vec.end());

    return col_reference(
      fixed_jump_iterator(_vec.begin() + offset, _n),
      fixed_jump_iterator(_vec.end() + offset, _n)
    );
  }

  
  col_iterator erase_cols(const const_col_iterator& first_col_it, const const_col_iterator& last_col_it) {
    difference_type del_cols_size = std::distance(first_col_it, last_col_it);
    _n -= del_cols_size;
   
    difference_type offset = first_col_it->begin().base() - _vec.cbegin();
    difference_type i = _n - offset - 1;

    auto it = _vec.begin();
    auto oit = _vec.begin();

    while(oit < _vec.end()) {
      if(++i == _n) {
        i = 0;
        oit += del_cols_size;
      }
      *it = std::move(*oit);
      ++it;
      ++oit;
    }

    _vec.erase(_vec.end() - del_cols_size * (_vec.size() / (_n + del_cols_size)), _vec.end());

    return col_reference(
      fixed_jump_iterator(_vec.begin() + offset, _n),
      fixed_jump_iterator(_vec.end() + offset, _n)
    );
  }



  // --------------------- Insert/Erase Generalization ----------------------- //

  // Make insert and erase, which call the appropriate insert_row or erase_row method

  //template 
  //auto insert(const row_iterator& where, const )




  // -------------------------- Element Access ----------------------------- //

  const_reference operator[](const difference_type id) const {
    auto beg = _vec.cbegin() + id * _n;
    return {beg, beg + _n};
  }

  reference operator[](const difference_type id) {
    auto beg = _vec.begin() + id * _n;
    return {beg, beg + _n};
  }

  row_reference row(const difference_type id) {
    auto beg = _vec.begin() + id * _n;
    return {beg, beg + _n};
  }
  const_row_reference row(const difference_type id) const {
    auto beg = _vec.begin() + id * _n;
    return {beg, beg + _n};
  }
  const_row_reference crow(const difference_type id) const {
    auto beg = _vec.cbegin() + id * _n;
    return {beg, beg + _n};
  }

  reverse_row_reference rrow(const difference_type id) {
    auto beg = _vec.rbegin() + id * _n;
    return {beg, beg + _n};
  }
  const_reverse_row_reference rrow(const difference_type id) const {
    auto beg = _vec.rbegin() + id * _n;
    return {beg, beg + _n};
  }
  const_reverse_row_reference crrow(const difference_type id) const {
    auto beg = _vec.crbegin() + id * _n;
    return {beg, beg + _n};
  }

  col_reference col(const difference_type id) {
    return { 
      fixed_jump_iterator(_vec.begin() + id, _n),
      fixed_jump_iterator(_vec.end() + id, _n)
    };
  }
  const_col_reference col(const difference_type id) const {
    return { 
      fixed_jump_iterator(_vec.begin() + id, _n),
      fixed_jump_iterator(_vec.end() + id, _n)
    };
  }
  const_col_reference ccol(const difference_type id) const {
    return { 
      fixed_jump_iterator(_vec.cbegin() + id, _n),
      fixed_jump_iterator(_vec.cend() + id, _n)
    };
  }

  reverse_col_reference rcol(const difference_type id) {
    return { 
      fixed_jump_iterator(_vec.rbegin() + id, _n),
      fixed_jump_iterator(_vec.rend() + id, _n)
    };
  }
  const_reverse_col_reference rcol(const difference_type id) const {
    return { 
      fixed_jump_iterator(_vec.rbegin() + id, _n),
      fixed_jump_iterator(_vec.rend() + id, _n)
    };
  }
  const_reverse_col_reference crcol(const difference_type id) const {
    return { 
      fixed_jump_iterator(_vec.crbegin() + id, _n),
      fixed_jump_iterator(_vec.crend() + id, _n)
    };
  }






  // --------------------------- Iterators ------------------------------ //

  /*row_iterator begin_rows() { return row_iterator({_vec.begin(), _vec.begin() + _n}, _n); }
  row_iterator end_rows() { return row_iterator({_vec.end(), _vec.end() + _n}, _n); }
  const_row_iterator begin_rows() const { return const_row_iterator({_vec.cbegin(), _vec.cbegin() + _n}, _n); }
  const_row_iterator end_rows() const { return const_row_iterator({_vec.cend(), _vec.cend() + _n}, _n); }
  const_row_iterator cbegin_rows() const { return const_row_iterator({_vec.cbegin(), _vec.cbegin() + _n}, _n); }
  const_row_iterator cend_rows() const { return const_row_iterator({_vec.cend(), _vec.cend() + _n}, _n); }

  reverse_row_iterator rbegin_rows() { return reverse_row_iterator({_vec.rbegin(), _vec.rbegin() + _n}, _n); }
  reverse_row_iterator rend_rows() { return reverse_row_iterator({_vec.rend(), _vec.rend() + _n}, _n); }
  const_reverse_row_iterator rbegin_rows() const { return const_reverse_row_iterator({_vec.crbegin(), _vec.crbegin() + _n}, _n); }
  const_reverse_row_iterator rend_rows() const { return const_reverse_row_iterator({_vec.crend(), _vec.crend() + _n}, _n); }
  const_reverse_row_iterator crbegin_rows() const { return const_reverse_row_iterator({_vec.crbegin(), _vec.crbegin() + _n}, _n); }
  const_reverse_row_iterator crend_rows() const { return const_reverse_row_iterator({_vec.crend(), _vec.crend() + _n}, _n); }*/

  row_iterator begin_rows() { return row_reference(_vec.begin(), _vec.begin() + _n); }
  row_iterator end_rows() { return row_reference(_vec.end(), _vec.end() + _n); }
  const_row_iterator begin_rows() const { return const_row_reference(_vec.begin(), _vec.begin() + _n); }
  const_row_iterator end_rows() const { return const_row_reference(_vec.end(), _vec.end() + _n); }
  const_row_iterator cbegin_rows() const { return const_row_reference(_vec.cbegin(), _vec.cbegin() + _n); }
  const_row_iterator cend_rows() const { return const_row_reference(_vec.cend(), _vec.cned() + _n); }

  reverse_row_iterator rbegin_rows() { return reverse_row_reference(_vec.rbegin(), _vec.rbegin() + _n); }
  reverse_row_iterator rend_rows() { return reverse_row_reference(_vec.rend(), _vec.rend() + _n); }
  const_reverse_row_iterator rbegin_rows() const { return const_reverse_row_reference(_vec.rbegin(), _vec.rbegin() + _n); }
  const_reverse_row_iterator rend_rows() const { return const_reverse_row_reference(_vec.rend(), _vec.rend() + _n); }
  const_reverse_row_iterator crbegin_rows() const { return const_reverse_row_reference(_vec.crbegin(), _vec.crbegin() + _n); }
  const_reverse_row_iterator crend_rows() const { return const_reverse_row_reference(_vec.crend(), _vec.crend() + _n); }


  col_iterator begin_cols() {
    return col_reference(
      fixed_jump_iterator(_vec.begin(), _n),
      fixed_jump_iterator(_vec.end(), _n)
    );
  }
  col_iterator end_cols() { 
    return col_reference(
      fixed_jump_iterator(_vec.begin() + _n, _n),
      fixed_jump_iterator(_vec.end() + _n, _n)
    );
  }
  const_col_iterator begin_cols() const {
    return const_col_reference(
      fixed_jump_iterator(_vec.begin(), _n),
      fixed_jump_iterator(_vec.end(), _n)
    );
  }
  const_col_iterator end_cols() const {
    return const_col_reference(
      fixed_jump_iterator(_vec.begin() + _n, _n),
      fixed_jump_iterator(_vec.end() + _n, _n)
    );
  }
  const_col_iterator cbegin_cols() const {
    return const_col_reference(
      fixed_jump_iterator(_vec.cbegin(), _n),
      fixed_jump_iterator(_vec.cend(), _n)
    );
  }
  const_col_iterator cend_cols() const {
    return const_col_reference(
      fixed_jump_iterator(_vec.cbegin() + _n, _n),
      fixed_jump_iterator(_vec.cend() + _n, _n)
    );
  }

  reverse_col_iterator rbegin_cols() {
    return reverse_col_reference(
      fixed_jump_iterator(_vec.rbegin(), _n),
      fixed_jump_iterator(_vec.rend(), _n)
    );
  }
  reverse_col_iterator rend_cols() { 
    return reverse_col_reference(
      fixed_jump_iterator(_vec.rbegin() + _n, _n),
      fixed_jump_iterator(_vec.rend() + _n, _n)
    );
  }
  const_reverse_col_iterator rbegin_cols() const {
    return const_reverse_col_reference(
      fixed_jump_iterator(_vec.rbegin(), _n),
      fixed_jump_iterator(_vec.rend(), _n)
    );
  }
  const_reverse_col_iterator rend_cols() const {
    return const_reverse_col_reference(
      fixed_jump_iterator(_vec.rbegin() + _n, _n),
      fixed_jump_iterator(_vec.rend() + _n, _n)
    );
  }
  const_reverse_col_iterator crbegin_cols() const {
    return const_reverse_col_reference(
      fixed_jump_iterator(_vec.crbegin(), _n),
      fixed_jump_iterator(_vec.crend(), _n)
    );
  }
  const_reverse_col_iterator crend_cols() const {
    return const_reverse_col_reference(
      fixed_jump_iterator(_vec.crbegin() + _n, _n),
      fixed_jump_iterator(_vec.crend() + _n, _n)
    );
  }


  /*col_iterator begin_cols() { return col_iterator(fixed_jump_iterator(_vec.begin(), _n), 1); }
  col_iterator end_cols() { return col_iterator(fixed_jump_iterator(_vec.end(), _n), 1); }
  const_col_iterator begin_cols() const { return col_iterator(fixed_jump_iterator(_vec.begin(), _n), 1); }
  const_col_iterator end_cols() const { return col_iterator(fixed_jump_iterator(_vec.end(), _n), 1); }
  const_col_iterator cbegin_cols() const { return col_iterator(fixed_jump_iterator(_vec.cbegin(), _n), 1); }
  const_col_iterator cend_cols() const { return col_iterator(fixed_jump_iterator(_vec.cend(), _n), 1); }

  col_iterator rbegin_cols() { return col_iterator(fixed_jump_iterator(_vec.rbegin(), _n), 1); }
  col_iterator rend_cols() { return col_iterator(fixed_jump_iterator(_vec.rend(), _n), 1); }
  const_col_iterator rbegin_cols() const { return col_iterator(fixed_jump_iterator(_vec.rbegin(), _n), 1); }
  const_col_iterator rend_cols() const { return col_iterator(fixed_jump_iterator(_vec.rend(), _n), 1); }
  const_col_iterator crbegin_cols() const { return col_iterator(fixed_jump_iterator(_vec.crbegin(), _n), 1); }
  const_col_iterator crend_cols() const { return col_iterator(fixed_jump_iterator(_vec.crend(), _n), 1); }*/

  
  // ------------------------------------------------------------------------- //

  Iterable<row_iterator> rows() { return {begin_rows(), end_rows()}; }
  Iterable<const_row_iterator> rows() const { return {begin_rows(), end_rows()}; }
  Iterable<const_row_iterator> crows() const { return {cbegin_rows(), cend_rows()}; }

  Iterable<reverse_row_iterator> rrows() { return {rbegin_rows(), rend_rows()}; }
  Iterable<const_reverse_row_iterator> rrows() const { return {rbegin_rows(), rend_rows()}; }
  Iterable<const_reverse_row_iterator> crrows() const { return {crbegin_rows(), crend_rows()}; }

  
  Iterable<col_iterator> cols() { return {begin_cols(), end_cols()}; }
  Iterable<const_col_iterator> cols() const { return {begin_cols(), end_cols()}; }
  Iterable<const_col_iterator> ccols() const { return {cbegin_cols(), cend_cols()}; }

  Iterable<reverse_col_iterator> rcols() { return {rbegin_cols(), rend_cols()}; }
  Iterable<const_reverse_col_iterator> rcols() const { return {rbegin_cols(), rend_cols()}; }
  Iterable<const_reverse_col_iterator> crcols() const { return {crbegin_cols(), crend_cols()}; }



  // --------------------------- Generalization ------------------------------ //

  iterator begin() { return begin_rows(); }
  iterator end() { return end_rows(); }
  const_iterator begin() const { return begin_rows(); }
  const_iterator end() const { return end_rows(); }
  const_iterator cbegin() const { return cbegin_rows(); }
  const_iterator cend() const { return cend_rows(); }

  /*reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator rend() const { return std::reverse_iterator(cbegin()); }
  const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }*/

  reverse_iterator rbegin() { return rbegin_rows(); }
  reverse_iterator rend() { return rend_rows(); }
  const_reverse_iterator rbegin() const { return rbegin_rows(); }
  const_reverse_iterator rend() const { return rend_rows(); }
  const_reverse_iterator crbegin() const { return crbegin_rows(); }
  const_reverse_iterator crend() const { return crend_rows(); }


  // ------------------------- Private Members ---------------------------- //

private:
  difference_type _n;       // size of a row / number of columns
  vec_type _vec; // flat vector for all elements

};


// Type Deduction Guides

//template <typename FlatContainer>
//Matrix(typename std::vector<typename FlatContainer::value_type>::difference_type size_of_row,
//             const FlatContainer &flat_data) -> Matrix<typename FlatContainer::value_type>;

template <typename FlatContainer>
Matrix(std::ptrdiff_t size_of_row, const FlatContainer &flat_data) -> Matrix<typename FlatContainer::value_type, typename FlatContainer::allocator_type>;

template <typename T>
Matrix(std::ptrdiff_t size_of_row, const std::initializer_list<T> &flat_data) -> Matrix<T, std::allocator<T>>;


template <typename OtherMatrix>
Matrix(const OtherMatrix& mat) -> Matrix<typename OtherMatrix::value_type::value_type, typename OtherMatrix::value_type::allocator_type>;

template <typename T>
Matrix(const std::initializer_list<std::initializer_list<T>>& mat) -> Matrix<T, std::allocator<T>>;



template <typename T, typename Alloc>
inline std::ostream& operator<<(std::ostream& stream, const Matrix<T, Alloc>& mat) {
  for(auto row : mat)
    stream << row << "\n";
  return stream;
}



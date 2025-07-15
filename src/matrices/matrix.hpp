#pragma once

#include <cassert>
#include <stdexcept>
#include <tuple> // for minimal_constructor specialization

#include "../iterators/tensor_iterator.hpp"

#include "../iterators/flat_iterator.hpp"
#include "../iterators/assign_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
// #include "../meta_func.hpp" // _if_t

// TODO: Add a boolean option to decide, if you want n to
// be dynamically updated or if you want it decided from
// initialization of Matrix object only

// TODO: Implement properly one of those Allocators to be able to call
// _vec.resize(), without initializing the data, so you can store
// non-default constructible elements in the Matrix class

// TODO: Imlement it somehow such that it is able to be like
// std::vector<std::tuple<int, std::vector<std::tuple<std::vector<char>, float>>>
// Using the Matrix and ZipContainer Wrappers
// It should be similar to Matrix<Matrix<char>>

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


// NOTE: If DYNAMIC_ROW_SIZE is false, then we need to always pass
// a row_size at construction, but when true, the row_size is
// determined at the first insert
template <typename VecType, bool DYNAMIC_ROW_SIZE = true>
class Matrix;


template <typename T, typename>
struct minimal_constructor;

template <typename VecType, bool DYNAMIC_ROW_SIZE>
class minimal_constructor<Matrix<VecType, DYNAMIC_ROW_SIZE>, void> {
  using mat_type = Matrix<VecType, DYNAMIC_ROW_SIZE>;
  using size_type = typename mat_type::size_type;
  static constexpr bool IS_REF = mat_type::IS_REF;
public:
  using type = std::conditional_t<
    IS_REF,
    std::conditional_t<
      DYNAMIC_ROW_SIZE, 
      std::tuple<VecType>,
      std::tuple<VecType, size_type>
    >,
    std::conditional_t<
      DYNAMIC_ROW_SIZE,
      std::tuple<>,
      std::tuple<size_type, size_type>
    >
  >;
};


// A Row-Major Rectangular Matrix-Like Container
// This is not a Mathematical matrix, this only
// provides base memory management for a matrix
template <typename VecType, bool DYNAMIC_ROW_SIZE>
class Matrix {
public:
  
  // using vec_type = VecType;
  using flat_cont = std::remove_cvref_t<VecType>;
  static_assert(is_container_v<flat_cont>);
  static_assert(is_vector_container_v<flat_cont>);
  // static_assert(std::is_base_of_v<std::random_access_iterator_tag,
  //   typename std::iterator_traits<typename flat_cont::iterator>::iterator_category>);

  // using T = typename VecType::value_type;
  using size_type = typename flat_cont::size_type;
  using difference_type = typename flat_cont::difference_type;
  using allocator_type = typename flat_cont::allocator_type;

private:

  static constexpr bool IS_REF = std::is_reference_v<VecType>;
  static constexpr bool IS_CONST = std::is_const_v<std::remove_reference_t<VecType>>;
  using __flat_cont_ref = std::conditional_t<IS_REF && !IS_CONST, flat_cont&, const flat_cont&>;

  static constexpr difference_type ZERO_INDEX = 0U;
  //using vec_type = std::vector<T, NoInitializeAllocator<Alloc>>;

  // template <typename, bool>
  // friend class Matrix;

public:

  using T = typename flat_cont::value_type;
  using flat_value_type = T;
  using flat_reference = typename flat_cont::reference;
  using flat_const_reference = typename flat_cont::const_reference;
  using flat_iterator = typename flat_cont::iterator;
  using flat_const_iterator = typename flat_cont::const_iterator;
  // using flat_reverse_iterator = typename flat_cont::reverse_iterator;
  // using flat_const_reverse_iterator = typename flat_cont::const_reverse_iterator;

  // BUG: !!!
  // with key_containers as flat, when you insert, it will insert keys,
  // instead of values, because their begin and end are key iterators
  using value_type = flat_cont;
  // BUG: !!! (Else, at insert, it will insert keys, instead of values)
  // using value_type = std::conditional_t<
  //   is_key_container_v<flat_cont>,
  //   typename flat_cont::base_type,
  //   flat_cont
  // >;
  // NOTE: This bug was when we tried to use a KeyCont Adaptor for as flat container

  using reference = TensorView<flat_iterator, FixedJumpPolicy<difference_type>>;
  using const_reference = TensorView<flat_const_iterator, FixedJumpPolicy<difference_type>>;
  // using reverse_reference = TensorIterable<flat_reverse_iterator, FixedJumpPolicy<difference_type>>;
  // using const_reverse_reference = TensorIterable<flat_const_reverse_iterator, FixedJumpPolicy<difference_type>>;
  
  using rvalue_reference = TensorView<std::move_iterator<flat_iterator>, FixedJumpPolicy<difference_type>>;

  using pointer = reference*;
  using const_pointer = const_reference*;
  // using reverse_pointer = reverse_reference*;
  // using const_reverse_pointer = const_reverse_reference*;

  using iterator = ViewToIterator<reference>;
  using const_iterator = ViewToIterator<const_reference>;
  // using reverse_iterator = IterableToIterator<reverse_reference>;
  // using const_reverse_iterator = IterableToIterator<const_reverse_reference>;

  // --------------------------- Capacity ------------------------------ //

  bool empty() const { return _vec.empty(); }
  size_type size() const { return _n == 0U ? 0U : _vec.size() / _n; }
  size_type capacity() const { return _n == 0U ? _vec.capacity() : _vec.capacity() / _n; }
  size_type row_size() const { return _n; }
  size_type col_size() const { return size(); }

  // ------------------------- Constructors ---------------------------- //

  template <bool COND = DYNAMIC_ROW_SIZE && !IS_REF,
    typename = std::enable_if_t<COND>>
  Matrix() : _n(ZERO_INDEX), _vec() {}

  // template <typename FlatContainer = std::initializer_list<T>>
  // Matrix(difference_type size_of_row, const FlatContainer &flat_data)
  //   : _n(size_of_row), _vec(flat_data.begin(), flat_data.end()) {
  //   if (size_of_row < 1)
  //     throw std::invalid_argument("Row size cannot be smaller than 1");
  //   if (flat_data.size() % size_of_row != 0)
  //     throw std::invalid_argument("The size of the flat data container, should be divisible by size_of_row");
  // }

  // template <typename FlatContainer = std::initializer_list<T>,
  //   bool COND = is_container_v<FlatContainer> && !IS_REF,
  //   typename = std::enable_if_t<COND>>
  // Matrix(difference_type num_of_rows, const FlatContainer &row)
  template <bool COND = !IS_REF, typename = std::enable_if_t<COND>>
  Matrix(size_type num_of_rows, const flat_cont &row)
  : _n(row.size()) {
    _vec.reserve(_n * num_of_rows);
    while(num_of_rows--)
      _vec.insert(_vec.end(), row.begin(), row.end());
  }

  // Number of Rows , Number of Columns
  template <bool COND = !IS_REF, typename = std::enable_if_t<COND>>
  Matrix(size_type num_of_rows, size_type num_of_cols, const T& val = T())
  : _n(num_of_cols), _vec(num_of_rows * num_of_cols, val) {}


  Matrix(__flat_cont_ref flat, size_type row_size)
  : _vec(flat), _n(row_size) {
    if (flat.size() % row_size != 0)
      throw std::out_of_range("The flat container size should be dividible by the row_size");
  }
  
  template <bool COND = DYNAMIC_ROW_SIZE, typename = std::enable_if_t<COND>>
  Matrix(__flat_cont_ref flat)
  : _vec(flat), _n(ZERO_INDEX) {}


  template <typename OtherMatrix = std::initializer_list<std::initializer_list<T>>,
    bool COND = is_matrix_v<OtherMatrix> && !IS_REF,
    typename = std::enable_if_t<COND>>
  Matrix(const OtherMatrix& mat)
    // : _n(ZERO_INDEX) {
    : _n(mat.first.size()) {
    // static_assert(is_matrix_v<OtherMatrix>);
    insert_range(end(), mat);
  }

  // template <typename OtherVec, typename = std::enable_if_t<std::is_constructible_v<typename OtherVec::value_type, T>>>
  // Matrix(const Matrix<OtherVec>& other)
  //   : _vec(other._vec.begin(), other._vec.end()), _n(other._n) {}


  const flat_cont& flat() const {
    return _vec;
  }

  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  flat_cont& flat() {
    return _vec;
  }

  // Iterable<flat_iterator> flat() {
  //   return {_vec.begin(), _vec.end()};
  // }


  // -------------------------- Modifiers ------------------------------ //

  void clear() {
    _vec.clear();
    // NOTE: Depending on if you check for _vec.empty() or _n == ZERO_INDEX
    //  in the insert/erase methods, you will need this uncommented !
    if constexpr (DYNAMIC_ROW_SIZE)
      _n = ZERO_INDEX;
  }

  void resize(size_type num_rows) {
    _vec.resize(num_rows * _n);
    if constexpr (DYNAMIC_ROW_SIZE)
      if (num_rows == 0)
        _n = ZERO_INDEX;
  }

  void resize_flat(size_type n) {
    _vec.resize(n);
    if constexpr (DYNAMIC_ROW_SIZE)
      if (n == 0)
        _n = ZERO_INDEX;
  }

  void reserve(size_type num_of_rows) {
    if(_n == 0)
      throw std::out_of_range("Cannot reserve number of rows if the size of the rows is unknown");
    _vec.reserve(_n * num_of_rows);
  }

  void reserve_flat(size_type num_of_vals) {
    _vec.reserve(num_of_vals);
  }

  //void reserve_flat(size_type num_of_values) {
  //  _vec.reserve(num_of_values);
  //}


  template <typename Container = std::initializer_list<T>>
  void push_back(const Container &new_row) {
    if constexpr (DYNAMIC_ROW_SIZE)
      if (_n == ZERO_INDEX)
      // if(_vec.empty())
        _n = new_row.size();

    if (!new_row.size()) { // initializer list doesnt have empty()
      _vec.insert(_vec.end(), _n, flat_value_type());
      // _vec.insert(_vec.end(), _n, {});
      return;
    }
    
    if (new_row.size() != _n)
      throw std::out_of_range(
          "Trying to add a larger size row in a matrix of fixed row size");
   
    _vec.insert(_vec.end(), new_row.begin(), new_row.end());
  }


  void pop_back() {
    _vec.erase(_vec.end() - _n - 1, _vec.end() - 1);
  }


  // template <typename Container = value_type>
  // template <typename Container = std::initializer_list<T>>
  // iterator insert(const_iterator where, const Container& new_row) {
  iterator insert(const_iterator where, const value_type& new_row) {
  // iterator insert(const_iterator where, const_reference new_row) {
    if constexpr (DYNAMIC_ROW_SIZE)
      if (_n == ZERO_INDEX)
      // if(_vec.empty())
        _n = new_row.size();

    if constexpr (std::is_default_constructible_v<flat_value_type>)
      if(!new_row.size())
        return iterator(_vec.insert(where->begin(), DefaultAssignIterator<flat_value_type>(0), DefaultAssignIterator<flat_value_type>(_n)), _n);

    if (new_row.size() != _n)
      throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");
    
    return iterator(_vec.insert(where->begin(), new_row.begin(), new_row.end()), _n);
  }

  iterator insert(const const_iterator& where, size_type num_of_rows, const value_type& new_row) {
  // iterator insert(const const_iterator& where, size_type num_of_rows, const_reference new_row) {
    // if constexpr (DYNAMIC_ROW_SIZE)
      if (_n == ZERO_INDEX)
      if(_vec.empty())
        _n = new_row.size();

    if constexpr (std::is_default_constructible_v<flat_value_type>)
      if(!new_row.size())
        return iterator(_vec.insert(where->begin(), _n * num_of_rows, flat_value_type()), _n);

    // if (!(new_row.size() == _n || new_row.size() == 0))
    if (new_row.size() != _n)
      throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    // NOTE: Here i would need repeat iterator
    
    // ValueAssignIterator<value_type> it(index_iterator(0LL), new_row);
    RefAssignIterator<value_type> it(IndexIterator(0LL), new_row);
    // return {_vec.insert(where->begin(), it, it + num_of_rows), _n};
    return insert(where, it, it + num_of_rows);
  }

  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  iterator insert(const_iterator where, IterType first_row_it, IterType last_row_it) {
    static_assert(is_range_v<typename std::iterator_traits<IterType>::value_type>);

    if constexpr (DYNAMIC_ROW_SIZE)
      if (_n == ZERO_INDEX)
      // if(_vec.empty())
        _n = first_row_it->size();

    for(auto it = first_row_it; it != last_row_it; ++it)
      if (it->size() != _n)
        throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    //return {_vec.insert(where.base(), flat_first, flat_last), _n};
    return iterator(
      _vec.insert(
        where->begin(),
        make_flat_iterator(first_row_it, last_row_it),
        make_flat_iterator(last_row_it)
      ), _n);
  }

  template <typename OtherMatrix = std::initializer_list<std::initializer_list<T>>>
  iterator insert_range(const_iterator where, const OtherMatrix& new_rows) {
    static_assert(is_range_v<OtherMatrix>);
    static_assert(is_range_v<typename OtherMatrix::value_type>);

    if constexpr (DYNAMIC_ROW_SIZE)
      if (_n == ZERO_INDEX)
      // if(_vec.empty())
        _n = new_rows.begin()->size();

    for(auto row : new_rows)
      if (row.size() != _n)
        throw std::out_of_range("Trying to add a larger size row in a matrix of fixed row size");

    return iterator(
      _vec.insert(
        where->begin(),
        make_flat_iterator(new_rows.begin(), new_rows.end()),
        make_flat_iterator(new_rows.end())
      ), _n);
  }


  iterator erase(const_iterator where) {
    if (_vec.size() == _n) // If only one row
      _n = ZERO_INDEX;

    //return _vec.erase(where.base(), where.base() + _n);
    return iterator(_vec.erase(where->begin(), where->end()), _n);
  }

  iterator erase(const_iterator first_row_it, const_iterator last_row_it) {
    // If the current number of elements is the same as the elements that are gonna be erased
    //if (_vec.size() == _n * std::distance(first_row_it, last_row_it))
    if (_vec.size() == std::distance(first_row_it->begin(), last_row_it->begin()))
      _n = ZERO_INDEX;

    //return _vec.erase(first_row_it.base(), last_row_it.base());
    return iterator(_vec.erase(first_row_it->begin(), last_row_it->begin()), _n);
  }


  // -------------------------- Element Access ----------------------------- //

  const_reference operator[](difference_type id) const {
    return const_reference(_vec.cbegin() + id * _n, _n);
  }
  reference operator[](difference_type id) {
    return reference(_vec.begin() + id * _n, _n);
  }

  const_reference at(difference_type id) const {
    if (id < 0 || id >= size())
      throw std::out_of_range("");
    return const_reference(_vec.cbegin() + id * _n, _n);
  }
  reference at(difference_type id) {
    if (id < 0 || id >= size())
      throw std::out_of_range("");
    return reference(_vec.begin() + id * _n, _n);
  }


  // --------------------------- Iterators ------------------------------ //


  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  iterator begin() { return iterator(_vec.begin(), _n); }
 
  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  iterator end() { return iterator(_vec.end(), _n); }
  
  const_iterator begin() const { return const_iterator(_vec.begin(), _n); }
  const_iterator end() const { return const_iterator(_vec.end(), _n); }
  const_iterator cbegin() const { return const_iterator(_vec.cbegin(), _n); }
  const_iterator cend() const { return const_iterator(_vec.cend(), _n); }


  /*template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  reverse_iterator rbegin() { return reverse_iterator(_vec.rbegin(), _n); }
 
  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  reverse_iterator rend() { return reverse_iterator(_vec.rend(), _n); }
  
  const_reverse_iterator rbegin() const { return const_reverse_iterator(_vec.rbegin(), _n); }
  const_reverse_iterator rend() const { return const_reverse_iterator(_vec.rend(), _n); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(_vec.crbegin(), _n); }
  const_reverse_iterator crend() const { return const_reverse_iterator(_vec.crend(), _n); }*/


  // ------------------------- Private Members ---------------------------- //

private:

  difference_type _n;       // size of a row / number of columns
  VecType _vec; // flat vector for all elements

};


// Type Deduction Guides

// template <typename T>
// Matrix(std::ptrdiff_t size_of_row, const std::initializer_list<T> &flat_data)
// -> Matrix<std::vector<T>>;
//
// template <typename ValIter>
// Matrix(std::ptrdiff_t size_of_row, const BaseIterable<ValIter> &flat_data)
// -> Matrix<std::vector<typename std::iterator_traits<ValIter>::value_type>>;
//
// template <typename FlatContainer>
// Matrix(std::ptrdiff_t size_of_row, const FlatContainer &flat_data)
// -> Matrix<FlatContainer>;
//
// template <typename OtherMatrix>
// Matrix(const OtherMatrix& mat)
// -> Matrix<typename OtherMatrix::value_type>;
//
// template <typename T>
// Matrix(const std::initializer_list<std::initializer_list<T>>& mat)
// -> Matrix<std::vector<T>>;


// template <typename VecType>
// inline std::ostream& operator<<(std::ostream& stream, const Matrix<VecType>& mat) {
//   for(auto row : mat)
//     stream << row << "\n";
//   return stream;
// }



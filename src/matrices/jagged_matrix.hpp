#pragma once

// #include "../iterators/iterable.hpp"
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/flat_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../meta_func.hpp"

#include "../iterators/index_iterator.hpp"
// #include "../iterators/iterator_iterator.hpp"
#include "../iterators/assign_iterator.hpp"
#include "../meta_func.hpp" // is_key_container_v

#include <cassert>
#include <stdexcept>

#include <vector>
#include <tuple> // for minimal_constructor specialization
#include <iterator>


template <typename ContType, bool IS_VEC_CONT = std::is_base_of_v<
  std::random_access_iterator_tag,
  typename std::iterator_traits<typename std::remove_cvref_t<ContType>::iterator>::iterator_category>
>
class JaggedMatrix;


template <typename T, typename>
struct minimal_constructor;

template <typename VecType>
class minimal_constructor<JaggedMatrix<VecType>, void> {
  using mat_type = JaggedMatrix<VecType>;
  // using size_type = typename mat_type::size_type;
  static constexpr bool IS_REF = mat_type::IS_REF;
public:
  using type = _if_t<
    IS_REF,
    std::tuple<VecType>,
    std::tuple<>
  >;
};



// TODO: Template this with BaseVectorType, so you can choose between
//  different container types that support the std::vector interface

// https://stackoverflow.com/questions/42668335/is-it-possible-to-have-a-matrix-with-different-row-sizes-in-c
// https://www.geeksforgeeks.org/jagged-arrays-in-cpp/
// https://stackoverflow.com/questions/18269123/ragged-and-jagged-arrays
// A Jagged/Ragged Array/Matrix Implementation
template <typename ContType>
class JaggedMatrix<ContType, true> {
  
  static constexpr bool IS_REF = std::is_reference_v<ContType>;
  static constexpr bool IS_CONST = std::is_const_v<std::remove_reference_t<ContType>>;
  
  // static const uint INVALID_INDEX = static_cast<uint>(-1);
  using values_vec_type = std::remove_cvref_t<ContType>;
  using __flat_cont_ref = _if_t<IS_REF && !IS_CONST, values_vec_type&, const values_vec_type&>;

  static_assert(is_container_v<values_vec_type>);
  static_assert(std::is_base_of_v<std::random_access_iterator_tag,
    typename std::iterator_traits<typename values_vec_type::iterator>::iterator_category>);
  // static_assert(is_vector_container_v<values_vec_type>);


  using ranges_vec_type = std::vector<typename values_vec_type::iterator::difference_type>;

  using T = typename values_vec_type::value_type;


  template <typename IterType>
  using _reference_type_t = TensorView<IterType, JumpByIndexDiff<typename ranges_vec_type::const_iterator>>;

public:

  // BUG: !!!
  // with key_containers as flat, when you insert, it will insert keys,
  // instead of values, because their begin and end are key iterators
  using value_type = values_vec_type;
  // BUG: !!! (Else, at insert, it will insert keys, instead of values)
  // using value_type = _if_t<
  //   is_key_container_v<values_vec_type>,
  //   typename values_vec_type::base_type,
  //   values_vec_type
  // >;
 
  using reference = _reference_type_t<typename values_vec_type::iterator>;
  using const_reference = _reference_type_t<typename values_vec_type::const_iterator>;
  using rvalue_reference = _reference_type_t<std::move_iterator<typename values_vec_type::iterator>>;
  
  using pointer = reference*;
  using const_pointer = const_reference*;

  using allocator_type = typename values_vec_type::allocator_type;
  // using size_type = uint;
  // using difference_type = std::ptrdiff_t;
  using size_type = typename values_vec_type::size_type;
  using difference_type = typename values_vec_type::difference_type;

  using iterator = ViewToIterator<reference>;
  using const_iterator = ViewToIterator<const_reference>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using flat_value_type = T;
  using flat_reference = typename values_vec_type::reference;
  using flat_const_reference = typename values_vec_type::const_reference;
  using flat_iterator = JumpIterator<typename values_vec_type::iterator, NoPolicy>;
  using flat_const_iterator = JumpIterator<typename values_vec_type::const_iterator, NoPolicy>;
  // using flat_iterator = typename values_vec_type::iterator;
  // using flat_const_iterator = typename values_vec_type::const_iterator;


protected:

  void shift(const_iterator where, difference_type offset) {
    auto id_it = _row_ranges.erase(where.policy().iter(), where.policy().iter());
    ++id_it;
    while(id_it != _row_ranges.end()) {
      *id_it += offset;
      ++id_it;
    }
  }

public:

  const values_vec_type& flat() const { return _vec; }

  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  values_vec_type& flat() { return _vec; }

  // Iterable<flat_const_iterator> flat() const {
  //   return Iterable(
  //     _vec.cbegin(),
  //     _vec.cend()
  //   );
  // }
  //  
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
  size_type capacity() const { return _row_ranges.capacity() - 1; }

  // ------------------------- Constructors ---------------------------- //

  template <bool COND = !IS_REF,
    typename = std::enable_if_t<COND>>
  JaggedMatrix() : _vec(), _row_ranges({0}) {}

  template <bool COND = !IS_REF,
    typename = std::enable_if_t<COND>>
  JaggedMatrix(size_type num_of_rows, size_type num_of_cols, const T& val = T()) 
  : _vec(num_of_rows * num_of_cols, val)
  , _row_ranges(
      JumpIterator(IndexIterator(difference_type(0)), num_of_cols),
      JumpIterator(IndexIterator(difference_type(num_of_cols * (num_of_rows + 1))), num_of_cols)
    ) {
    // In case that (0, 0) is given
    if (num_of_cols == 0 && num_of_rows == 0)
      // _row_ranges.resize(1, 0);
      _row_ranges.push_back(0);
  }

  template <typename IterType,
    bool COND = !IS_REF && is_iterator_v<IterType>,
    typename = std::enable_if_t<COND>>
  JaggedMatrix(IterType first, IterType last) : JaggedMatrix() {
    insert(end(), first, last);
  }

  template <typename Container = std::initializer_list<T>,
    bool COND = !IS_REF && is_container_v<Container>,
    typename = std::enable_if_t<COND>>
  JaggedMatrix(size_type num_of_rows, const Container& row = {}) : JaggedMatrix() {
    // reserve(num_of_rows);
    // reserve_flat(num_of_rows * row.size());
    // while(num_of_rows--)
    //   push_back(row);
    insert(end(), num_of_rows, row);
  }


  JaggedMatrix(__flat_cont_ref flat)
  : _vec(flat)
  , _row_ranges({difference_type(0)}) {}

  // JaggedMatrix(__flat_cont_ref flat)
  // : _vec(flat)
  // , _row_ranges({
  //   difference_type(0),
  //   difference_type(flat.size())
  // }) {
  //   if (flat.empty())
  //     flat.pop_back();
  // }
  
  // JaggedMatrix(__flat_cont_ref flat, size_type row_size)
  // : _vec(flat)
  // , _row_ranges(
  //     JumpIterator(index_iterator(difference_type(0)), row_size),
  //     JumpIterator(index_iterator(difference_type(row_size + flat.size())), row_size)
  //   ) {}
 
  JaggedMatrix(__flat_cont_ref flat, const ranges_vec_type& ranges)
  : _vec(flat), _row_ranges(ranges) {}
  

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
  
  // TODO: Fix this
  // void resize(size_type num_rows) {
  //   _row_ranges.resize(1 + num_rows);
  // }
  // 
  // void resize_flat(size_type num_val) {
  //   _vec.resize(num_val);
  // }

  void push_back(const value_type &new_row) {
  // template <typename Container = std::initializer_list<T>>
  // void push_back(const Container &new_row) {
    //_row_ranges.push_back({ static_cast<uint>(_vec.size()), static_cast<uint>(new_row.size()) });
    // static_assert(is_container_v<Container>);

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


  // template <typename Container = std::initializer_list<T>>
  // iterator insert(const const_iterator& where, const Container& new_row) {
  iterator insert(const_iterator where, const value_type& new_row) {
    // static_assert(is_container_v<Container>);

    int n = new_row.size();
    auto val_it = _vec.insert(where->begin(), new_row.begin(), new_row.end());

    auto id_it = _row_ranges.insert(where.policy().iter() + 1, *(where.policy().iter()));
    for(auto it = id_it; it != _row_ranges.end(); ++it) {
      *it += n;
      // std::advance(*it, n);
    }

    return iterator(
      val_it,
      typename ranges_vec_type::const_iterator(std::prev(id_it))
    );
  }


  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  iterator insert(const_iterator where, IterType first_row_it, IterType last_row_it) {
    // static_assert(is_iterator_v<IterType>);
    static_assert(is_container_v<typename std::iterator_traits<IterType>::value_type>);

    auto val_it = _vec.insert(
      where->begin(),
      make_flat_iterator(first_row_it, last_row_it),
      make_flat_iterator(last_row_it)
    );

    // NOTE: Here you can use boost::transform iterator
    // that transforms the given rows to row.size() + *where->policy().iter()
    auto id_it = _row_ranges.insert(where.policy().iter() + 1, std::distance(first_row_it, last_row_it), *(where.policy().iter()));

    auto iit = id_it;
    difference_type n = 0;
    for(auto it = first_row_it; it != last_row_it; ++it) {
      n += it->size();
      *(iit++) += n;
    }

    while(iit != _row_ranges.end())
      *(iit++) += n;

    return iterator(
      val_it,
      typename ranges_vec_type::const_iterator(std::prev(id_it))
    );
  }
 
  iterator insert(const_iterator where, size_type num_of_rows, const value_type& new_row) {
    if (new_row.empty()) {
      auto id_it = where.policy().iter();
      auto new_id_it = _row_ranges.insert(id_it, num_of_rows, *id_it);
      return iterator(
        _vec.erase(where->begin(), where->begin()),
        typename ranges_vec_type::const_iterator(new_id_it)
      );
    }
    // RefAssignIterator<value_type> it(index_iterator(0LL), new_row);
    RefAssignIterator<value_type> it(IndexIterator(0LL), new_row);
    return insert(where, it, it + num_of_rows);
  }


  iterator erase(const_iterator where) {
    int n = where->size();
    auto val_it = _vec.erase(where->begin(), where->end());
    auto id_it = _row_ranges.erase(where.policy().iter() + 1);
    
    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;

    return iterator(
      val_it,
      typename ranges_vec_type::const_iterator(std::prev(id_it))
    );
  }

  iterator erase(const_iterator first_row_it, const_iterator last_row_it) {
    // int n = *(last_row_it.policy().iter()) - *(first_row_it.policy().iter());
    int n = last_row_it.base() - first_row_it.base();
    
    auto val_it = _vec.erase(first_row_it.base(), last_row_it.base());
    auto id_it = _row_ranges.erase(first_row_it.policy().iter() + 1, last_row_it.policy().iter() + 1);

    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;

    return iterator(
      val_it,
      typename ranges_vec_type::const_iterator(std::prev(id_it))
    );
  }

  // ------------------------- In Row insert/erase methods ----------------- //

  // TODO: Implement insert_in with arguments without the row_it
  // iterating branchlessly through the row range ids using arrange_traits<flat_cont>

  flat_iterator insert_in(
    const_iterator row_it,
    flat_const_iterator val_it,
    const T& val
  ) {
    assert(row_it->begin() <= val_it && val_it <= row_it->end());
    // auto it = row_it.policy().iter();
    auto it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
    ++it;

    for(; it != _row_ranges.end(); ++it)
      ++(*it);

    return _vec.insert(val_it.base(), val);
    // return _vec.insert(val_it, val);
  }


  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  flat_iterator insert_in(
    const_iterator row_it,
    flat_const_iterator val_it,
    IterType first,
    IterType last
  ) {
    assert(row_it->begin() <= val_it && val_it <= row_it->end());
    // auto it = row_it.policy().iter(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
    ++it;

    auto n = std::distance(first, last);
    for(; it != _row_ranges.end(); ++it)
      *it += n;

    return _vec.insert(val_it.base(), first, last);
    // return _vec.insert(val_it, first, last);
  }


  flat_iterator erase_in(
    const_iterator row_it,
    flat_const_iterator val_it
  ) {
    assert(row_it->begin() <= val_it && val_it < row_it->end());
    // auto it = row_it.policy().iter(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
    ++it;

    for(; it != _row_ranges.end(); ++it)
      --(*it);

    return _vec.erase(val_it.base());
    // return _vec.erase(val_it);
  }


  flat_iterator erase_in(
    const_iterator row_it,
    flat_const_iterator from,
    flat_const_iterator to
  ) {
    assert(row_it->begin() <= from && from < row_it->end());
    assert(row_it->begin() <= to && to < row_it->end());
    // auto it = row_it.policy().iter(); // We need to remove the constness of the given row_it
    auto it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
    ++it;

    auto n = std::distance(from, to);
    for(; it != _row_ranges.end(); ++it)
      *it -= n;

    return _vec.erase(from.base(), to.base());
    // return _vec.erase(from, to);
  }


  // -------------------------- Element Access ----------------------------- //

  const_reference operator[](difference_type id) const {
    return *(cbegin() + id);
  }

  reference operator[](difference_type id) {
    return *(begin() + id);
  }

  const_reference at(difference_type id) const {
    if (id < 0 || id >= _row_ranges.size())
      throw std::out_of_range("Out of range at() method");
    return *(cbegin() + id);
  }

  reference at(difference_type id) {
    if (id < 0 || id >= _row_ranges.size())
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


  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  iterator begin() { return iterator(_vec.begin(), _row_ranges.cbegin()); }
 
  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  iterator end() { return iterator(_vec.end(), _row_ranges.cend() - 1); }
  
  const_iterator begin() const { return const_iterator(_vec.begin(), _row_ranges.cbegin()); }
  const_iterator end() const { return const_iterator(_vec.end(), _row_ranges.cend() - 1); }
  const_iterator cbegin() const { return const_iterator(_vec.cbegin(), _row_ranges.cbegin()); }
  const_iterator cend() const { return const_iterator(_vec.cend(), _row_ranges.cend() - 1); }


  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
 
  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  
  const_reverse_iterator rbegin() const { return std::reverse_iterator(end()); }
  const_reverse_iterator rend() const { return std::reverse_iterator(begin()); }
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

  ContType _vec;
  // values_vec_type _vec; // flat vector for all elements
  ranges_vec_type _row_ranges;

};


// ====================================================================== //


template <typename ContType>
class JaggedMatrix<ContType, false> {
  
  static_assert(is_container_v<ContType>);
  static_assert(std::is_base_of_v<std::bidirectional_iterator_tag,
    typename std::iterator_traits<typename ContType::iterator>::iterator_category>);

  using values_vec_type = ContType;
  using ranges_vec_type = std::vector<typename values_vec_type::iterator>;
  using T = typename ContType::value_type;

  template <typename IterType>
  using _reference_type_t = TensorView<IterType, JumpByEqualNext<typename ranges_vec_type::const_iterator>>;

public:

  using value_type = values_vec_type;
 
  using reference = _reference_type_t<typename values_vec_type::iterator>;
  using const_reference = _reference_type_t<typename values_vec_type::const_iterator>;
  
  using pointer = reference*;
  using const_pointer = const_reference*;

  using allocator_type = typename ContType::allocator_type;
  // using size_type = uint;
  // using difference_type = std::ptrdiff_t;
  using size_type = typename ContType::size_type;
  using difference_type = typename ContType::difference_type;

  using iterator = ViewToIterator<reference>;
  using const_iterator = ViewToIterator<const_reference>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using flat_value_type = T;
  using flat_reference = typename values_vec_type::reference;
  using flat_const_reference = typename values_vec_type::const_reference;
  // using flat_iterator = JumpIterator<typename values_vec_type::iterator, NoPolicy>;
  // using flat_const_iterator = JumpIterator<typename values_vec_type::const_iterator, NoPolicy>;
  using flat_iterator = typename values_vec_type::iterator;
  using flat_const_iterator = typename values_vec_type::const_iterator;


  const values_vec_type& flat() const { return _vec; }
  values_vec_type& flat() { return _vec; }

  // Iterable<flat_const_iterator> flat() const {
  //   return Iterable(
  //     _vec.cbegin(),
  //     _vec.cend()
  //   );
  // }
  //  
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

  JaggedMatrix() : _vec(), _row_ranges({_vec.end()}) {}

  JaggedMatrix(size_type num_of_rows, size_type num_of_cols, const T& val = T()) {
    _row_ranges.reserve(num_of_rows + 1);
    while(num_of_rows--)
      _row_ranges.push_back(_vec.insert(_vec.end(), num_of_cols, val));
    _row_ranges.push_back(_vec.end());
  }

  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  JaggedMatrix(const IterType& first, const IterType& last) : JaggedMatrix() {
    insert(end(), first, last);
  }

  template <typename Container = std::initializer_list<T>,
    typename = std::enable_if_t<is_container_v<Container>>>
  JaggedMatrix(size_type num_of_rows, const Container& row = {}) {
    _row_ranges.reserve(num_of_rows + 1);
    while(num_of_rows--)
      _row_ranges.push_back(_vec.insert(_vec.end(), row.begin(), row.end()));
    _row_ranges.push_back(_vec.end());
  }


  // -------------------------- Modifiers ------------------------------ //

  void clear() {
    _vec.clear();
    _row_ranges.assign({_vec.end()});
  }

  void reserve(size_type num_rows) {
    _row_ranges.reserve(1 + num_rows);
  }

  
  // TODO: Fix this
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
    static_assert(is_container_v<Container>);
    _row_ranges.insert(_row_ranges.end() - 1,
      _vec.insert(_vec.end(), new_row.begin(), new_row.end())
    );
  }

  void pop_back() {
    _vec.erase(
      *(_row_ranges.end() - 2),
      _vec.end()
    );
    _row_ranges.erase(_row_ranges.end() - 2);
  }

  // IMPLEMENT INSERT AND ERASE HERE //

  template <typename Container = std::initializer_list<T>>
  iterator insert(const const_iterator& where, const Container& new_row) {
    static_assert(is_container_v<Container>);

    auto val_it = _vec.insert(where->begin(), new_row.begin(), new_row.end());

    typename ranges_vec_type::const_iterator it_it =
      _row_ranges.insert(where.policy().iter(), val_it);

    return {
      val_it,
      it_it,
    };
  }

  // TODO: (1)
  template <typename IterType>
  iterator insert(const const_iterator& where, IterType first_row_it, IterType last_row_it) {
    static_assert(is_iterator_v<IterType>);
    static_assert(is_container_v<typename std::iterator_traits<IterType>::value_type>);

    // auto val_it = where->begin().base();
    auto val_it = _vec.erase(where->begin().base(), where->begin().base());
    auto it_it = where.policy().iter(); // ranges const_iterator
    // auto it_it = _row_ranges.erase(where.policy().iter(), where.policy().iter()); // ranges const_iterator

    for(int i = 0; i < 2; ++i) {
    // while(last_row_it != first_row_it) {
      val_it = _vec.insert(val_it, last_row_it->begin(), last_row_it->end());
      _row_ranges.insert(it_it, val_it);
      --last_row_it;
    }

    /*// if (first_row_it != last_row_it)
    auto new_val_it = _vec.insert(where->begin(), first_row_it->begin(), first_row_it->end());
    auto new_it_it = _row_ranges.insert(where.policy().iter(), new_val_it);
    ++first_row_it;

    // auto val_it = new_val_it;

    // while(first_row_it != last_row_it) {
      auto val_it = _vec.insert(where->begin(), first_row_it->begin(), first_row_it->end());
      _row_ranges.insert(where.policy().iter(), val_it);
      ++first_row_it;
    // }*/
    // std::cout << std::distance(*where.policy().iter(), _vec.end()) << "\n";

    return {
      val_it,
      typename ranges_vec_type::const_iterator(it_it),
    };
  }


  iterator erase(const const_iterator& where) {
    auto val_it = _vec.erase(where->begin(), where->end());
    auto it_it = _row_ranges.erase(where.policy().iter());
    
    return {
      val_it,
      typename ranges_vec_type::const_iterator(it_it),
    };
  }

  // TODO: (2)
  iterator erase(const const_iterator& first_row_it, const const_iterator& last_row_it) {
    // int n = *(last_row_it.policy().iter()) - *(first_row_it.policy().iter());
    int n = last_row_it.base() - first_row_it.base();
    
    auto val_it = _vec.erase(first_row_it.base(), last_row_it.base());
    auto id_it = _row_ranges.erase(first_row_it.policy().iter() + 1, last_row_it.policy().iter() + 1);

    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;

    return {
      val_it,
      typename ranges_vec_type::const_iterator(id_it - 1),
    };
  }

  // ------------------------- In Row insert/erase methods ----------------- //


  flat_iterator insert_in(
    const const_iterator& row_it,
    const flat_const_iterator& val_it,
    const T& val
  ) {
    auto new_val_it = _vec.insert(val_it, val);
    if (val_it == row_it->begin()) {
      auto it_it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
      *it_it = new_val_it;
    }
    return new_val_it;
  }


  template <typename IterType>
  flat_iterator insert_in(
    const const_iterator& row_it,
    const flat_const_iterator& val_it,
    const IterType& first,
    const IterType& last
  ) {
    auto new_val_it = _vec.insert(val_it, first, last);
    if (val_it == row_it->begin()) {
      auto it_it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
      *it_it = new_val_it;
    }
    return new_val_it;
  }


  flat_iterator erase_in(
    const const_iterator& row_it,
    const flat_const_iterator& val_it
  ) {
    auto new_val_it = _vec.erase(val_it);
    if (val_it == row_it->begin()) {
      auto it_it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
      *it_it = new_val_it;
    }
    return new_val_it;
  }


  flat_iterator erase_in(
    const const_iterator& row_it,
    const flat_const_iterator& from,
    const flat_const_iterator& to
  ) {
    auto new_from = _vec.erase(from, to);
    if (from == row_it->begin()) {
      auto it_it = _row_ranges.erase(row_it.policy().iter(), row_it.policy().iter());
      *it_it = new_from;
    }
    return new_from;
  }


  // -------------------------- Element Access ----------------------------- //

  const_reference operator[](difference_type id) const {
    return *(cbegin() + id);
  }

  reference operator[](difference_type id) {
    return *(begin() + id);
  }

  const_reference at(difference_type id) const {
    if (id < 0 || id >= _row_ranges.size())
      throw std::out_of_range("Out of range at() method");
    return *(cbegin() + id);
  }

  reference at(difference_type id) {
    if (id < 0 || id >= _row_ranges.size())
      throw std::out_of_range("Out of range at() method");
    return *(begin() + id);
  }

  const_reference operator()(difference_type id) const {
    return *(cbegin() + id);
  }

  reference operator()(difference_type id) {
    return *(begin() + id);
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


  // ------------------------- Private Members ---------------------------- //

private:

  values_vec_type _vec; // flat vector for all elements
  ranges_vec_type _row_ranges;

};




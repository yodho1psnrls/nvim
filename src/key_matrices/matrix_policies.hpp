#pragma once

#include "../iterators/index_iterator.hpp"
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../key_kernel/keys.hpp"
#include "../cont_adaptors/traits.hpp"

#include <vector>
#include <type_traits>


template <typename KeyContainer>
class RectKeyMatRows;


template <typename KeyPol, typename = void>
struct is_matrix_policy : public std::false_type {};

template <typename KeyPol>
struct is_matrix_policy<
  KeyPol,
  std::void_t<typename KeyPol::flat_cont>
> : public std::true_type {};

template <typename KeyPol>
constexpr bool is_matrix_policy_v = is_matrix_policy<KeyPol>::value;


/*template <typename KeyContainer>
class RectKeyMatRows {
public:
  
  using flat_type = KeyContainer;
  // static_assert(is_container_v<flat_type>);
  static_assert(is_key_container_v<flat_type>);

  using flat_key_type = typename flat_type::key_type;
  using flat_key_iterator = typename flat_type::key_iterator;
  
  using difference_type = typename handle_traits<flat_key_type>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  // using key_type = typename flat_type::key_type;
  // using key_iterator = typename flat_type::key_iterator;
  using key_type = size_type;
  using key_iterator = index_iterator<size_type>;

  using value_type = std::vector<flat_key_type>;
  using reference = TensorIterable<jump_iterator<flat_key_iterator, FixedJumpPolicy<difference_type>>>;
  using pointer = reference&;
  using iterator = IterableToIterator<reference>;

  explicit RectKeyMatRows(KeyContainer& flat, size_type row_size)
  : _flat(flat), _n(row_size) {}


  size_type size() const {
    return _flat.size() / _n;
  }


  iterator begin() const {
    return {index_iterator(size_type(0)), _n};
  }

  iterator end() const {
    return {index_iterator(_flat.size()), _n};
  }

  reference at(key_type key) const {
    return begin()[key];
  }


  iterator insert(iterator it) const {
    return {_flat.insert(it->begin(), _n), _n};
  }
  
  iterator insert(iterator it, size_type num_of_rows) const {
    return {_flat.insert(it->begin(), _n * num_of_rows), _n};
  }

  iterator erase(iterator it) {
    return {_flat.erase(it->begin(), it->end()), _n};
  }

  iterator erase(iterator from, iterator to) {
    return {_flat.erase(from->begin(), to->begin()), _n};
  }

private:

  KeyContainer& _flat;
  difference_type _n; // Row Size

};*/


template <typename KeyContainer>
class RectKeyMatRows {
public:

  using flat_cont = KeyContainer;
  // static_assert(is_container_v<flat_type>);
  static_assert(is_key_container_v<flat_cont>);

  using flat_key_type = typename flat_cont::key_type;
  using flat_key_iterator = typename flat_cont::key_iterator;
  
  using difference_type = typename handle_traits<flat_key_type>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  // using key_type = typename flat_type::key_type;
  // using key_iterator = typename flat_type::key_iterator;
  using key_type = size_type;
  using key_iterator = index_iterator<size_type>;

  using value_type = std::vector<flat_key_type>;
  using reference = TensorIterable<jump_iterator<flat_key_iterator, FixedJumpPolicy<difference_type>>>;
  using pointer = reference&;
  using iterator = IterableToIterator<reference>;


  // explicit RectKeyMatRows(flat_cont& flat)
  // : _flat(flat), _n(-1) {}

  RectKeyMatRows(flat_cont& flat, size_type row_size)
  : _flat(flat), _n(row_size) {}

  using constructor_args = type_list<flat_cont&, size_type>;
  // using additional_arg = size_type;


  RectKeyMatRows& operator=(const RectKeyMatRows& other) {
    _n = other._n;
  }

  size_type size() const {
    return _flat.size() / _n;
  }

  key_iterator begin() const {
    return key_iterator(size_type(0));
  }
  
  key_iterator end() const {
    return key_iterator(size());
  }

  reference operator[](key_type key) const {
    return {flat_key_iterator(key * _n), _n};
  }

  reference at(key_type key) const {
    return {flat_key_iterator(key * _n), _n};
  }


  key_iterator insert(key_iterator key_it) const {
    _flat.insert(at(*key_it).begin(), _n);
    return key_it;
  }
  
  key_iterator insert(key_iterator key_it, size_type num_of_rows) const {
    _flat.insert(at(*key_it).begin(), _n * num_of_rows);
    return key_it;
  }

  key_iterator erase(key_iterator key_it) {
    _flat.erase(at(*key_it).begin(), at(*key_it).end());
  }

  key_iterator erase(key_iterator from, key_iterator to) {
    _flat.erase(at(*from).begin(), at(*to).begin());
  }

  // flat_key_iterator insert_at(key_iterator row_key, flat_key_iterator flat_key) {
  //   return _flat.inser
  // }

private:

  KeyContainer& _flat;
  difference_type _n; // Row Size

};


template <typename KeyContainer>
class JaggedKeyMatRows {
public:
  
  using flat_type = KeyContainer;
  // static_assert(is_container_v<flat_type>);
  static_assert(is_key_container_v<flat_type>);

  using flat_key_type = typename flat_type::key_type;
  using flat_key_iterator = typename flat_type::key_iterator;
  
  using difference_type = typename handle_traits<flat_key_type>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  // using key_type = typename flat_type::key_type;
  // using key_iterator = typename flat_type::key_iterator;
  using key_type = size_type;
  using key_iterator = index_iterator<size_type>;

  using value_type = std::vector<flat_key_type>;
  using reference = TensorIterable<jump_iterator<flat_key_iterator, FixedJumpPolicy<difference_type>>>;
  using pointer = reference&;
  using iterator = IterableToIterator<reference>;

  explicit JaggedKeyMatRows(KeyContainer& flat, size_type row_size)
  : _flat(flat), _n(row_size) {}


  size_type size() const {
    return _flat.size() / _n;
  }

  key_iterator begin() const {
    return key_iterator(size_type(0));
  }
  
  key_iterator end() const {
    return key_iterator(size());
  }

  reference operator[](key_type key) const {
    return {flat_key_iterator(key * _n), _n};
  }

  reference at(key_type key) const {
    return {flat_key_iterator(key * _n), _n};
  }
  
  // reference keys(key_type key) const {
  //   return {flat_key_iterator(key * _n), _n};
  // }
  // 
  // reference values(key_type key) const {
  //   return {flat_key_iterator(key * _n), _n};
  // }


  key_iterator insert(key_iterator key_it) const {
    _flat.insert(at(*key_it).begin(), _n);
    return key_it;
  }
  
  key_iterator insert(key_iterator key_it, size_type num_of_rows) const {
    _flat.insert(at(*key_it).begin(), _n * num_of_rows);
    return key_it;
  }

  key_iterator erase(key_iterator key_it) {
    _flat.erase(at(*key_it).begin(), at(*key_it).end());
  }

  key_iterator erase(key_iterator from, key_iterator to) {
    _flat.erase(at(*from).begin(), at(*to).begin());
  }

private:

  KeyContainer& _flat;
  difference_type _n; // Row Size

};




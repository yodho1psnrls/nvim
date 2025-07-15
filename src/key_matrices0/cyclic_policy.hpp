#pragma once
#include "../cont_adaptors/traits.hpp" // is_key_container_v
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../iterators/index_iterator.hpp"
#include <vector>


// template <typename KeyContainer>
class CyclicMatrix {

  template <typename KeyCont>
  using cont_to_key_type = typename KeyCont::key_type;
  
  template <typename KeyCont>
  using cont_to_key_iter = typename KeyCont::key_iterator;

public:

  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  // using flat_key_type = ValKey;
  // using flat_key_iterator = typename handle_traits<ValKey>::iterator;
  
  using key_type = size_t;
  using key_iterator = index_iterator<size_t>;
  using value_type = std::vector<key_type>;

  template <typename flat_key_iterator>
  using reference = TensorIterable<jump_iterator<flat_key_iterator, FixedJumpPolicy<difference_type>>>;
  
  template <typename flat_key_iterator>
  using pointer = reference<flat_key_iterator>*;
 
  // using iterator = IterableToIterator<reference>;

  // explicit RectKeyMatRows(flat_cont& flat)
  // : _flat(flat), _n(-1) {}

  CyclicMatrix(size_type row_size)
  : _n(row_size) {}

  // template <typename KC>
  // KeyRectMat(const KeyRectMat<KC>& other)
  // : _ker(other.flat()), _n(other.row_size()) {}

  // using constructor_args = type_list<flat_cont&, size_type>;
  // using additional_arg = size_type;

  // NOTE: No need for modifying
  /*KeyRectMat& operator=(const KeyRectMat& other) {
    _n = other._n;
    if constexpr (!IS_REF)
      _ker = other._ker;
  }
  // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  KeyRectMat& operator=(KeyRectMat&& other) {
    _n = other._n;
    if constexpr (!IS_REF)
      _ker = std::move(other._ker);
  }*/


  template <typename KeyCont>
  size_type size(const KeyCont& flat) const {
    static_assert(is_key_container_v<KeyCont>);
    return flat.size() / _n;
  }
  
  // Number of Columns
  size_type row_size() const {
    return _n;
  }

  template <typename KeyCont>
  key_iterator begin(const KeyCont&) const {
    static_assert(is_key_container_v<KeyCont>);
    return key_iterator(size_type(0));
  }
  
  template <typename KeyCont>
  key_iterator end(const KeyCont& cont) const {
    static_assert(is_key_container_v<KeyCont>);
    return key_iterator(size_type(this->size(cont)));
  }

  // template <typename flat_key_iterator>
  // reference<flat_key_iterator> at(key_type key) const {
  //   return reference(flat_key_iterator(key * _n), _n);
  // }
  
  template <typename KeyCont>
  reference<cont_to_key_iter<KeyCont>> at(const KeyCont&, key_type key) const {
    static_assert(is_key_container_v<KeyCont>);
    using KeyIter = cont_to_key_iter<KeyCont>;
    return reference<KeyIter>(
      KeyIter(key * _n),
      _n
    );
  }

  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it) const {
    static_assert(is_key_container_v<KeyCont>);
    flat.insert(
      // at<cont_to_key_iter<KeyCont>>(*key_it).begin(),
      at(flat, *key_it).begin(),
      _n
    );
    return key_it;
  }
  
  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it, size_type num_of_rows) const {
    static_assert(is_key_container_v<KeyCont>);
    flat.insert(
      // at<cont_to_key_iter<KeyCont>>(*key_it).begin(),
      at(flat, *key_it).begin(),
      _n * num_of_rows
    );
    return key_it;
  }

  template <typename KeyCont>
  key_iterator erase(KeyCont& flat, key_iterator key_it) const {
    static_assert(is_key_container_v<KeyCont>);
    flat.erase(
      // at<cont_to_key_iter<KeyCont>>(*key_it).begin(),
      // at<cont_to_key_iter<KeyCont>>(*key_it).end()
      at(flat, *key_it).begin(),
      at(flat, *key_it).end()
    );
    return key_it;
  }

  template <typename KeyCont>
  key_iterator erase(KeyCont& flat, key_iterator from, key_iterator to) const {
    static_assert(is_key_container_v<KeyCont>);
    return flat.erase(
      // at<cont_to_key_iter<KeyCont>>(*from).begin(),
      // at<cont_to_key_iter<KeyCont>>(*to).begin()
      at(flat, *from).begin(),
      at(flat, *to).begin()
    );
    return from;
  }

  // flat_key_iterator insert_at(key_iterator row_key, flat_key_iterator flat_key) {
  //   return _flat.inser
  // }

private:

  // difference_type _n; // Row Size
  size_type _n; // Row Size // NOTE: UnAmbiguous for KeyVector<int>::insert(it, size_type)

};



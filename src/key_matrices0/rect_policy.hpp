#pragma once

#include "../iterators/index_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../iterators/tensor_iterator.hpp"
// #include "../key_kernel/keys.hpp"
#include "../containers/key_traits.hpp" // is_key_container_v
// #include "../meta_func.hpp"

#include <vector>
// #include <type_traits>


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


/*template <typename KeyContainer>
class KeyRectMat {

  static constexpr bool IS_REF = std::is_reference_v<KeyContainer>;
  static constexpr bool IS_CONST = std::is_const_v<std::remove_reference_t<KeyContainer>>;

public:

  using flat_cont = std::remove_cvref_t<KeyContainer>;
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

  // template <
  //   bool COND = IS_REF && !IS_CONST,
  //   typename = std::enable_if_t<COND>
  // >
  // KeyRectMat(flat_cont& flat, size_type row_size)
  // : _flat(flat), _n(row_size) {}
  //
  // template <
  //   bool COND = !IS_REF || IS_REF && IS_CONST,
  //   typename = std::enable_if_t<COND>
  // >
  // // RectKeyMat(_if_t<std::is_reference_v<KeyContainer>, KeyContainer, const flat_cont&> flat, size_type row_size)
  // KeyRectMat(const flat_cont& flat, size_type row_size)
  // : _flat(flat), _n(row_size) {}

  KeyRectMat(_if_t<IS_REF && !IS_CONST, flat_cont&, const flat_cont&> flat, size_type row_size)
  : _flat(flat), _n(row_size) {}

  template <typename KC>
  KeyRectMat(const KeyRectMat<KC>& other)
  : _flat(other.flat()), _n(other.row_size()) {}

  using constructor_args = type_list<flat_cont&, size_type>;
  // using additional_arg = size_type;

  // NOTE: No need for modifying
  // KeyRectMat& operator=(const KeyRectMat& other) {
  //   _n = other._n;
  //   if constexpr (!IS_REF)
  //     _flat = other._flat;
  // }
  // // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  // KeyRectMat& operator=(KeyRectMat&& other) {
  //   _n = other._n;
  //   if constexpr (!IS_REF)
  //     _flat = std::move(other._flat);
  // }

  template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  flat_cont& flat() { return _flat; }

  const flat_cont& flat() const { return _flat; }

  size_type size() const {
    return _flat.size() / _n;
  }
  
  // Number of Columns
  size_type row_size() const {
    return _n;
  }

  key_iterator begin() const {
    return key_iterator(size_type(0));
  }
  
  key_iterator end() const {
    return key_iterator(size());
  }

  reference operator[](key_type key) const {
    return reference(flat_key_iterator(key * _n), _n);
  }

  reference at(key_type key) const {
    return reference(flat_key_iterator(key * _n), _n);
  }


  // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  key_iterator insert(key_iterator key_it) {
    static_assert(!IS_CONST);
    _flat.insert(at(*key_it).begin(), _n);
    return key_it;
  }
  
  // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  key_iterator insert(key_iterator key_it, size_type num_of_rows) {
    static_assert(!IS_CONST);
    _flat.insert(at(*key_it).begin(), _n * num_of_rows);
    return key_it;
  }

  // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  key_iterator erase(key_iterator key_it) {
    static_assert(!IS_CONST);
    _flat.erase(at(*key_it).begin(), at(*key_it).end());
  }

  // template <bool COND = !IS_CONST, typename = std::enable_if_t<COND>>
  key_iterator erase(key_iterator from, key_iterator to) {
    static_assert(!IS_CONST);
    _flat.erase(at(*from).begin(), at(*to).begin());
  }

  // flat_key_iterator insert_at(key_iterator row_key, flat_key_iterator flat_key) {
  //   return _flat.inser
  // }

private:

  // KeyContainer& _flat;
  KeyContainer _flat;

  // difference_type _n; // Row Size
  size_type _n; // Row Size // NOTE: UnAmbiguous for KeyVector<int>::insert(it, size_type)

};*/
  
/*KeyVector<std::vector<int>> vec = {1, 2, 3, 4, 5, 6};
// KeyVector<std::vector<Foo<int>>> vec;
KeyRectMat<KeyVector<std::vector<int>>&> mat(vec, 2);
KeyRectMat<KeyVector<std::vector<int>>> omat = mat;

mat.insert(mat.end());
mat.insert(mat.end(), 69);
// mat.insert(mat.end(), 15);

for(auto row : omat)
  std::cout << omat[row] << "\n";*/


// template <typename KeyContainer>
class KeyRectMatPolicy {

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
  using key_iterator = IndexIterator<size_t>;
  using value_type = std::vector<key_type>;

  template <typename flat_key_iterator>
  using reference = TensorView<flat_key_iterator, FixedJumpPolicy<difference_type>>;
  
  template <typename flat_key_iterator>
  using pointer = reference<flat_key_iterator>*;
 
  template <typename flat_key_iterator>
  using iterator = ViewToIterator<reference<flat_key_iterator>>;

  // explicit RectKeyMatRows(flat_cont& flat)
  // : _flat(flat), _n(-1) {}

  KeyRectMatPolicy(size_type row_size)
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
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    return flat.size() / _n;
  }
  
  // Number of Columns
  size_type row_size() const {
    return _n;
  }

  template <typename KeyCont>
  key_iterator begin(const KeyCont&) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    return key_iterator(size_type(0));
  }
  
  template <typename KeyCont>
  key_iterator end(const KeyCont& cont) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    return key_iterator(size_type(this->size(cont)));
  }

  // template <typename flat_key_iterator>
  // reference<flat_key_iterator> at(key_type key) const {
  //   return reference(flat_key_iterator(key * _n), _n);
  // }
  
  template <typename KeyCont>
  reference<cont_to_key_iter<KeyCont>> at(const KeyCont&, key_type key) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    using KeyIter = cont_to_key_iter<KeyCont>;
    return reference<KeyIter>(
      KeyIter(key * _n),
      _n
    );
  }

  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    flat.insert(
      // at<cont_to_key_iter<KeyCont>>(*key_it).begin(),
      at(flat, *key_it).begin(),
      _n
    );
    return key_it;
  }
  
  template <typename KeyCont>
  key_iterator insert(KeyCont& flat, key_iterator key_it, size_type num_of_rows) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
    flat.insert(
      // at<cont_to_key_iter<KeyCont>>(*key_it).begin(),
      at(flat, *key_it).begin(),
      _n * num_of_rows
    );
    return key_it;
  }

  template <typename KeyCont>
  key_iterator erase(KeyCont& flat, key_iterator key_it) const {
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
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
    // static_assert(is_key_container_v<KeyCont>);
    static_assert(has_cont_traits_v<KeyCont>);
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




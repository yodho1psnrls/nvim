#pragma once

#include "../iterators/index_iterator.hpp"
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../key_kernel/keys.hpp"
#include "../cont_adaptors/traits.hpp"

#include <vector>
#include <type_traits>

template <typename KeyPol, typename = void>
struct is_matrix_policy : public std::false_type {};

template <typename KeyPol>
struct is_matrix_policy<
  KeyPol,
  std::void_t<typename KeyPol::flat_cont>
> : public std::true_type {};

template <typename KeyPol>
constexpr bool is_matrix_policy_v = is_matrix_policy<KeyPol>::value;


template <
  typename KeyMultCont,
  typename Policy 
>
class KeyMatrix {

  // static constexpr bool IS_REF = std::is_reference_v<KeyContainer>;
  // static constexpr bool IS_CONST = std::is_const_v<std::remove_reference_t<KeyContainer>>;

public:

  using policy_type = Policy;
  using flat_cont = KeyMultCont;
  // using flat_cont = KeyKernel::template key_to_cont_type<ValKey>;
  // using flat_cont = std::remove_cvref_t<KeyContainer>;
  // static_assert(is_container_v<flat_type>);
  static_assert(is_key_container_v<flat_cont>);
  // static_assert(is_matrix_policy_v<Policy>);

  using flat_key_type = typename flat_cont::key_type;
  using flat_key_iterator = typename flat_cont::key_iterator;
  
  using difference_type = typename handle_traits<flat_key_type>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  using key_type = typename Policy::key_type;
  using key_iterator = typename Policy::key_iterator;

  using value_type = std::vector<flat_key_type>;
  using reference = Policy::template reference<flat_key_iterator>;
  using pointer = Policy::template reference<flat_key_iterator>;
  using iterator = Policy::template reference<flat_key_iterator>;
  
  template <bool COND = std::is_default_constructible_v<policy_type>,
    typename = std::enable_if_t<COND>>
  explicit KeyMatrix(flat_cont& flat)
  : _flat(flat), _pol() {}

  explicit KeyMatrix(flat_cont& flat, const policy_type& pol)
  : _flat(flat), _pol(pol) {}

  // template <typename KC>
  // KeyRectMat(const KeyRectMat<KC>& other)
  // : _ker(other.flat()), _n(other.row_size()) {}

  using constructor_args = type_list<flat_cont&, size_type>;
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

  flat_cont& flat() { return _flat; }
  const flat_cont& flat() const { return _flat; }

  flat_cont& policy() { return _pol; }
  const flat_cont& policy() const { return _pol; }

  size_type size() const {
    return _pol.size(_flat);
  }
  
  // Number of Columns
  // size_type row_size() const {
  //   return _n;
  // }

  key_iterator begin() const {
    return _pol.begin(_flat);
  }
  
  key_iterator end() const {
    return _pol.end(_flat);
  }

  reference operator[](key_type key) const {
    return _pol.at(_flat, key);
  }

  reference at(key_type key) const {
    return _pol.at(_flat, key);
  }

  key_iterator insert(key_iterator key_it) {
    return _pol.insert(_flat, key_it);
  }
  
  key_iterator insert(key_iterator key_it, size_type num_of_rows) {
    return _pol.insert(_flat, key_it, num_of_rows);
  }

  key_iterator erase(key_iterator key_it) {
    return _pol.erase(_flat, key_it);
  }

  key_iterator erase(key_iterator from, key_iterator to) {
    return _pol.erase(_flat, from, to);
  }
  
  flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_it) {
    return _pol.insert_in(_flat, key_it, flat_it);
  }
  
  flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_it, size_type num_of_rows) {
    return _pol.insert_in(_flat, key_it, flat_it, num_of_rows);
  }

  flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_it) {
    return _pol.erase_in(_flat, key_it, flat_it);
  }

  flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_from, flat_key_iterator flat_to) {
    return _pol.erase_in(_flat, key_it, flat_from, flat_to);
  }

  // flat_key_iterator insert_at(key_iterator row_key, flat_key_iterator flat_key) {
  //   return _flat.inser
  // }

private:

  flat_cont& _flat;
  [[no_unique_address]] policy_type _pol;

};


// template <typename KeyCont>
// KeyMatrix(KeyCont&, size_t)
// -> KeyMatrix<KeyCont, KeyRectMatPolicy>;



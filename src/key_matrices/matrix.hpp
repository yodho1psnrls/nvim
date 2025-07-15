#pragma once

#include <utility>  // forward

#include "../containers/key_traits.hpp"
#include "rect_policy.hpp"


template <
  typename FlatCont,
  template <typename> class KeyMatT = KeyRectMatPolicy
>
class MatrixImpl {

public:

  using flat_key_iter = typename key_cont_traits<FlatCont>::key_iterator;

private:

  using KeyMat = KeyMatT<flat_key_iter>;
  using Traits = key_cont_traits<FlatCont>;

public:

  using size_type = typename KeyMat::size_type;
  using difference_type = typename KeyMat::difference_type;

  using value_type = FlatCont;
  // using reference = 

  // using iterator = LookupIterator

  template <
    typename ...Args,
    typename = std::enable_if_t<std::is_constructible_v<
      KeyMat, flat_key_iter, flat_key_iter, std::decay_t<Args>...>
    >
  >
  MatrixImpl(const FlatCont& flat, Args&&... args)
  : _flat(flat)
  , KeyMat(
      Traits::begin(_flat),
      Traits::end(_flat),
      std::forward<Args>(args)...
    ) {}
  
  template <
    typename ...Args,
    typename = std::enable_if_t<std::is_constructible_v<
      KeyMat, flat_key_iter, flat_key_iter, std::decay_t<Args>...>
    >
  >
  MatrixImpl(FlatCont&& flat, Args&&... args)
  : _flat(std::move(flat))
  , KeyMat(
      Traits::begin(_flat),
      Traits::end(_flat),
      std::forward<Args>(args)...
    ) {}


  bool empty() const {
    return _kmat.empty();
  }

  difference_type size() const {
    return _kmat.size();
  }

  difference_type flat_size() const {
    return _kmat.flat_size();
  }

  difference_type row_size() const {
    return _kmat.row_size();
  }


  auto begin() {
    // return 
  }

  auto end() {
    // return 
  }


  // iterator insert(const_iterator pos, const_reference val) {
  // }


private:

  FlatCont _flat;
  KeyMat _kmat;

};


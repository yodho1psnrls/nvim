#pragma once

#include "../meta_func.hpp"
// #include "crtp_iterator.hpp"
#include "transform_iterator.hpp"
// #include "../containers/key_traits.hpp"

#include <iterator>
#include <type_traits>
#include <utility> // declval
// #include <functional> // std::reference_wrapper

// TODO: Implement LookupFunctor and MakeLookupFunctor to
// result in IteratorLookupFunctor, when given a View Type


// LookUpType is anything that supports operator[]
/*template <typename KeyIter, typename LookUpType>
class lookup_iterator : public KeyIter {

  KeyIter _it;
  LookUpType& lookup;
};*/


// LookUpType is any random acess iterator that supports operator[]
/*template <typename KeyIter, typename LookUpType>
class iterator_lookup_iterator : public KeyIter {

  using LookIC = typename std::iterator_traits<LookUpType>::iterator_category; 
  static_assert(std::is_base_of_v<std::random_access_iterator_tag, LookIC>);
  
  //static_assert(std::is_integral_v<typename std::iterator_traits<KeyIter>::value_type>);
  
  using LookDT = typename std::iterator_traits<LookUpType>::difference_type;
  using KeyV = typename std::iterator_traits<KeyIter>::value_type;
  static_assert(std::is_constructible_v<LookDT, KeyV>);


  // NOTE: KeyIter should be const_iterator, there is no problem if its not,
  // but there is no reason for it to not be const, because the constness of
  // this iterator depends on the lookuptype and the KeyIter only gives us
  // an index, that we do not change here

  LookUpType _lookup;

public:

  using value_type = typename LookUpType::value_type;
  using reference = typename LookUpType::reference;
  using pointer = typename LookUpType::pointer;


  iterator_lookup_iterator(const KeyIter& it, const LookUpType& lookup)
    : KeyIter(it), _lookup(lookup) {}
  
  reference operator*() const { return _lookup[KeyIter::operator*()]; }
  pointer operator->() const { return &(_lookup[KeyIter::operator*()]); }
  
};


// LookUpType is any container that supports operator[]
template <typename KeyIter, typename LookUpType>
class container_lookup_iterator  : public KeyIter {

  // The container may not be with integer key/index/handle so, we should not assert that
  //static_assert(std::is_integral_v<typename std::iterator_traits<KeyIter>::value_type>);

  // NOTE: KeyIter should be const_iterator, there is no problem if its not,
  // but there is no reason for it to not be const, because the constness of
  // this iterator depends on the lookuptype and the KeyIter only gives us
  // an index, that we do not change here

  LookUpType& _lookup;

public:

  using value_type = typename LookUpType::value_type;
  using reference = std::conditional_t<std::is_const_v<LookUpType>, typename LookUpType::const_reference, typename LookUpType::reference>;
  using pointer = std::conditional_t<std::is_const_v<LookUpType>, typename LookUpType::const_pointer, typename LookUpType::pointer>;


  container_lookup_iterator(const KeyIter& it, LookUpType& lookup)
    : KeyIter(it), _lookup(lookup) {}
  
  reference operator*() const { return _lookup[KeyIter::operator*()]; }
  pointer operator->() const { return &(_lookup[KeyIter::operator*()]); }
  
};*/


template <typename ContType>
class ContainerLookupFunctor {
protected:
  
  // static_assert(!std::is_reference_v<std::remove_cv_t<LookupType>>);
  static_assert(!std::is_reference_v<ContType>); // no need for remove_cv
  static_assert(!std::is_pointer_v<ContType>); // no need for remove_cv

  // NOTE: It may be not a sequence container, but a kernel that gives you
  // different values depending on the key that you lookup from it
  // static_assert(is_container_v<ContType>);
  // static_assert(!is_view_v<ContType>);

  // LookupType& _map;
  // std::reference_wrapper<LookupType> _map;
  ContType* _cont;

  template <typename>
  friend class ContainerLookupFunctor;
  
  // using Traits = key_cont_traits<std::remove_const_t<LookupType>>;

public:

  // using reference = std::conditional_t<
  //  std::is_const_v<LookupType>,
  //  typename LookupType::const_reference,
  //  typename LookupType::reference
  // >;

  // using reference = std::conditional_t<
  //  std::is_const_v<LookupType>,
  //  typename Traits::const_reference,
  //  typename Traits::reference
  // >;

  using difference_type = typename ContType::difference_type;
  using reference = typename ContType::reference;

  ContainerLookupFunctor(ContType& cont)
  : _cont(&cont) {}

  // template <typename LT, typename = std::enable_if_t<std::is_convertible_v<LT&, LookupType&>>>
  // ContainerLookupFunctor(const ContainerLookupFunctor<LT>& other)
  // : _map(other._map) {}
  
  template <bool COND = std::is_const_v<ContType>, typename = std::enable_if_t<COND>>
  ContainerLookupFunctor(const ContainerLookupFunctor<std::remove_const_t<ContType>>& other)
  : _cont(other._cont) {}
  // : _map(&other.lookup()) {}
  
  // ContainerLookupFunctor& operator=(const ContainerLookupFunctor& other) = default;

  // NOTE: Let it be templated key, because the lookup container
  // could be a kernel which has multiple key types and returned value types accordingly
  template <typename KeyType>
  decltype(auto) operator()(KeyType&& key) const {
  // auto&& operator()(KeyType&& key) const {
    // if constexpr (has_cont_traits_v<ContType>) {
    //   using Traits = key_cont_traits<std::remove_const_t<ContType>>;
    //   return Traits::at(*_cont, std::forward<KeyType>(key));
    // }
    // else
      return _cont->at(std::forward<KeyType>(key));
      // return const_cast<LookupType&>(_map).at(std::forward<KeyType>(key));
  }

  // reference operator()(typename Traits::key_type key) const {
  //  return Traits::at(_cont, key);
  // }

  ContType& lookup() const { return *_cont; }
  // LookupType& lookup() const { return _cont.get(); }
  
  // const LookupType& lookup() const { return _cont; }
  // LookupType& lookup() { return _cont; }
  
  // LookupType& lookup() const { return const_cast<LookupType&>(_cont); }
  // LookupType& lookup() const { return const_cast<ThisType&>(*this)._cont; }

};


template <typename IterType>
class IteratorLookupFunctor {
protected:
  
  // static_assert(!std::is_const_v<std::remove_reference_t<LookupType>>);
  static_assert(std::is_same_v<IterType, std::remove_cvref_t<IterType>>);
  static_assert(is_iterator_v<IterType>);
  static_assert(std::is_base_of_v<
    std::random_access_iterator_tag,
    typename std::iterator_traits<IterType>::iterator_category>
  );

  IterType _it;

  template <typename>
  friend class IteratorLookupFunctor;

public:

  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  using reference = typename std::iterator_traits<IterType>::reference;

  IteratorLookupFunctor(const IterType& map)
  : _it(map) {}
  
  template <typename LT, typename =
    std::enable_if_t<std::is_constructible_v<IterType, LT>>>
    // std::enable_if_t<std::is_convertible_v<LT, LookupType>>>
  IteratorLookupFunctor(const IteratorLookupFunctor<LT>& other)
  : _it(other._it) {}

  // template <typename ViewType, typename =
  //   std::enable_if_t<is_view_v<ViewType>>>
  // IteratorLookupFunctor(const ViewType& view)
  // : _it(view.begin()) {
  //   // static_assert(std::is_convertible_v<typename ViewType::iterator, LookupType>);
  //   // static_assert(std::is_constructible_v<LookupType, typename ViewType::iterator>);
  // }

  // template <typename ContType>
  template <typename ContType, typename = std::enable_if_t<
    std::is_constructible_v<IterType, range_to_iterator_t<ContType>>>>
  IteratorLookupFunctor(const ContainerLookupFunctor<ContType>& cl)
  : _it(cl.lookup().begin()) {
    // using IT = std::remove_cvref_t<decltype(std::declval<ContType>().begin())>;
    // using IC = typename std::iterator_traits<IT>::iterator_category;
    // static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC>);
    // static_assert(std::is_constructible_v<IterType, IT>);
  }

  // template <typename KeyType>
  // reference operator()(const KeyType& key) const {
  reference operator()(difference_type key) const {
    return _it[key];
  }
  
  using lookup_type = IterType;
  const IterType& lookup() const { return _it; }

};


template <typename LookupType>
using LookupFunctor = std::conditional_t<
  is_iterator_v<LookupType>,
  IteratorLookupFunctor<LookupType>,
  // std::conditional_t<is_container_v<std::remove_cvref_t<LookupType>>
  ContainerLookupFunctor<LookupType>
>;

/*template <typename LookupType>
using LookupFunctor = std::conditional_t<
  // is_container_v<std::remove_cvref_t<LookupType>>,
  is_container_v<LookupType>,
  ContainerLookupFunctor<LookupType>,
  std::conditional_t<
    is_iterator_v<LookupType>,
    IteratorLookupFunctor<LookupType>,
    std::conditional_t<
      is_view_v<LookupType>,
      IteratorLookupFunctor<typename LookupType::iterator>
    >
  >
  // std::conditional_t<is_iterator_v<std::remove_cvref_t<LookupType>>
>;*/


template <typename LookupType, typename = std::enable_if_t<
  is_container_v<std::remove_const_t<LookupType>>>>
inline auto make_lookup_functor(LookupType& arg) {
  return LookupFunctor<LookupType>(arg);
}

template <typename LookupType, typename = std::enable_if_t<
  is_iterator_v<LookupType>>>
  // is_iterator_v<LookupType> || is_view_v<LookupType>>>
inline auto make_lookup_functor(const LookupType& arg) {
  return LookupFunctor<LookupType>(arg);
}


template <typename KeyIter, typename LookupType>
using LookupIterator =
// std::conditional_t<
//  is_redundant_lookup_v<LookupType>,
//  KeyIter,
  TransformIterator<
    KeyIter,
    LookupFunctor<LookupType>
  >;
// >;


// template <typename LookupType>
// using __lookup_arg_type = std::conditional_t<is_iterator_v<LookupType>, const LookupType&, LookupType&>;


template <typename KeyIter, typename LookupArg>
inline auto make_lookup_iterator(const KeyIter& key_it, LookupArg&& lookup) {
  // using LA = std::remove_cvref_t<LookupArg>;
  // static_assert(is_iterator_v<LA> || is_range_v<LA>);

  // if constexpr (is_redundant_lookup_v<std::remove_cvref_t<LookupArg>>)
  //  return key_it;
  // else
    return TransformIterator(key_it, make_lookup_functor(std::forward<LookupArg>(lookup)));
}


// template <typename IntType, typename Value, typename Alloc>
// inline typename std::vector<Value, Alloc>::iterator make_lookup_iterator(
//   const IndexIterator<IntType>& key_it, typename std::vector<Value, Alloc>::iterator lookup
// ) {
//   return lookup + *key_it;
// }

// template <typename IntType, typename LookupIter, typename =
//   std::enable_if_t<std::random_access_iterator<LookupIter>>>
// inline LookupIter make_lookup_iterator(const IndexIterator<IntType>& key_it, LookupIter lookup) {
//   return lookup += *key_it;
// }




/*template <typename KeyIter, typename LookupType>
class LookupIterator 
: public CrtpIterator<
    LookupIterator<KeyIter, LookupType>,
    TransformIterator<
      KeyIter,
      std::conditional_t<
        is_iterator_v<LookupType>,
        __IteratorLookupFunctor<LookupType>,
        __ContainerLookupFunctor<LookupType>
      >
    >
  > {

  using ThisType = LookupIterator<KeyIter, LookupType>;

  using IterType = 
    TransformIterator<
      KeyIter,
      std::conditional_t<
        is_iterator_v<LookupType>,
        __IteratorLookupFunctor<LookupType>,
        __ContainerLookupFunctor<LookupType>
      >
    >;

  using CrtpType = CrtpIterator<ThisType, IterType>;

  template <typename, typename>
  friend class LookupIterator;

public:

  using key_iterator = KeyIter;
  using key_type = typename std::iterator_traits<KeyIter>::value_type;
  using key_reference = typename std::iterator_traits<KeyIter>::reference;
  using key_pointer = typename std::iterator_traits<KeyIter>::pointer;

  // using lookup_type = LookupType;

  using CrtpType::CrtpType;

  template <typename LT>
  LookupIterator(const LookupIterator<KeyIter, LT>& other)
  : CrtpType(other.key_it(), other.lookup()) {}

  // Only key assignment (preserves the previous MapType)
  // template <typename KI = KeyIter, typename = std::enable_if_t<is_iterator_v<KI>>>
  // ThisType& operator=(const KI& key_it) {
  //   _it = key_it;
  //   return *this;
  // }

  // const LookupType& lookup() const { return _map; }
  // LookupType& lookup() { return _map; }
  // const KeyIter& key_it() const { return _it; }
  // key_reference key() const { return *_it; }
  
  // const LookupType& lookup() const { return const_cast(CrtpType::base().functor()).lookup(); }
  // const LookupType& lookup() const { return CrtpType::base().functor(); }
  const IterType::functor_type& lookup() const { return CrtpType::base().functor(); }
  const key_iterator& key_it() const { return CrtpType::base().base(); }
  key_reference key() const { return *key_it(); }

};*/


/*template <typename KeyIter, typename LookupType>
LookupIterator(
  const KeyIter&,
  const LookupType&
) ->
LookupIterator<
  KeyIter,
  std::conditional_t<
    is_container_v<LookupType>,
    const LookupType,
    LookupType
  >
>;


template <typename KeyIter, typename LookupType>
LookupIterator(
  const KeyIter&,
  LookupType&
) ->
LookupIterator<
  KeyIter,
  LookupType
>;*/



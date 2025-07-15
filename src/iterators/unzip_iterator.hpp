#pragma once

// #include "crtp_iterator.hpp"
#include "transform_iterator.hpp"
// #include "zip_iterator.hpp"
// #include "lookup_iterator.hpp"

#include "view.hpp"


// NOTE: It would be nice to not have this and just use std::get<ID>
// but since our TransformIterator has hard time with functors with multiple overloads
// we need this specific one which works only on tuples and integral indexes
template <size_t ID>
struct TupleGetFunc {
  template <typename TupType>
  // std::tuple_element_t<ID, TupType>& operator()(TupType&& tup) const { // Doesnt work when you pass tuple reference
  decltype(auto) operator()(TupType&& tup) const {
    return std::get<ID>(std::forward<TupType>(tup));
  }
};

// -------------------------------------------------------------------------- //

template <size_t ID, typename IterType>
inline auto make_unzip_iterator(const IterType& it) {
// inline unzip_iterator<IterType, ID> make_unzip_iterator(const IterType& it) {
  // return unzip_iterator<IterType, ID>(it);
  return TransformIterator<IterType, TupleGetFunc<ID>>(it);
}

template <typename Iter, size_t ID>
using UnzipIterator = decltype(make_unzip_iterator<ID>(std::declval<Iter>()));

template <size_t ID, typename Range>
inline auto unzipped(Range&& range) {
  static_assert(is_range_v<std::remove_reference_t<Range>>);
  return View(
    make_unzip_iterator<ID>(std::forward<Range>(range).begin()),
    make_unzip_iterator<ID>(std::forward<Range>(range).end())
  );
}

// -------------------------------------------------------------------------- //

/*template <typename Iter, size_t ID>
using unzip_iterator = TransformIterator<Iter, TupleGetFunc<ID>>;


template <typename ...Iters, size_t ID>
class TransformIterator<zip_iterator<Iters...>, TupleGetFunc<ID>>
: public CrtpIterator<
    TransformIterator<zip_iterator<Iters...>, TupleGetFunc<ID>>,
    nth_type_t<ID, Iters...>
  > {

  using ThisType = TransformIterator<zip_iterator<Iters...>, TupleGetFunc<ID>>;
  using IterType = nth_type_t<ID, Iters...>;
  using CrtpType = CrtpIterator<ThisType, IterType>;

// protected:
public:

  // BUG: WHY THIS HAS NO EFFECT ???
  // friend CrtpType;
  // friend class CrtpIterator<ThisType, IterType>;
  
  explicit TransformIterator(IterType it)
  : CrtpType(it) {}

public:

  explicit TransformIterator(const zip_iterator<Iters...>& zip_it)
  : CrtpType(std::get<ID>(zip_it.tuple())) {}
  
  // Needed to fullfil the TransformIterator interface
  explicit TransformIterator(const zip_iterator<Iters...>& zip_it, const TupleGetFunc<ID>&)
  : CrtpType(std::get<ID>(zip_it.tuple())) {}

  using base_type = IterType;
  using functor_type = TupleGetFunc<ID>;

  const base_type& base() const { return CrtpType::iter(); }
  static functor_type functor() { return functor_type(); }

};*/



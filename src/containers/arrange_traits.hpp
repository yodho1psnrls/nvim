#pragma once

#include "../meta_func.hpp"
#include "key_traits.hpp"


// NOTE: Each of the order/shift traits classes have one to one
// mapping to arrange_traits

// NOTE: We wont touch the exact erased key value !!
// TODO: Maybe it would be better if we invalidate the erased values

// TODO: Make them also not branchless if they happen to be
//  non-random-access iterators

struct ShiftArrange {

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void will_insert(
    Key& key_to_update,
    const Cont&,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {
    key_to_update += count * (key_to_update > inserted_key);
  }

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void was_erased(
    Key& key_to_update,
    const Cont&,
    const Key& erased_key,
    std::ptrdiff_t count = 1
  ) {
    // if (*first >= at_key && *first < at_key + n) {
    //   first = keys.erase(first);
    //   continue;
    // }
    key_to_update -= count * (key_to_update >= erased_key + count);
  }

};


// TODO: Implement it correctly when count != 1

struct ChainArrange {

  // NOTE: You will need to know the size of the container
  // that the keys point to

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void will_insert(
    Key& key_to_update,
    const Cont& cont,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {
    // key_to_update +=
    //   (key_to_update == inserted_key) * (cont.size() - key_to_update);
    key_to_update +=
      (key_to_update >= inserted_key && key_to_update < inserted_key + count)
      * (cont.size() - inserted_key);
  }

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void was_erased(
    Key& key_to_update,
    const Cont& cont,
    const Key& erased_key,
    std::ptrdiff_t count = 1
  ) {
    // key_to_update +=
    //   (key_to_update == cont.size()) * (erased_key - key_to_update);
    key_to_update +=
      (key_to_update >= cont.size() && key_to_update < cont.size() + count)
      * (erased_key - cont.size());
  }
  
};


struct NoShiftArrange {

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void will_insert(
    Key& key_to_update,
    const Cont&,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {}

  template <
    typename Cont,
    typename Key = typename key_cont_traits<Cont>::key_type
  >
  static void was_erased(
    Key& key_to_update,
    const Cont&,
    const Key& erased_key,
    std::ptrdiff_t count = 1
  ) {}
  
};


template <typename BaseVec>
struct UnorderedVector;


template <typename VecType>
struct arrange_traits : public
  _if_t<
    is_vector_container_v<VecType>,
    ShiftArrange,
    NoShiftArrange
  > {};


template <typename BaseVec>
struct arrange_traits<
  UnorderedVector<BaseVec>
> : public ChainArrange {};





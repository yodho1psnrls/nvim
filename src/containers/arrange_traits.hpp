#pragma once

#include "../meta_func.hpp"


// NOTE: Each of the order/shift traits classes have one to one
// mapping to arrange_traits

// NOTE: We wont touch the exact erased key value !!


struct ShiftArrange {

  template <typename Key>
  static void will_insert(
    Key& key_to_update,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {
    key_to_update += count * (key_to_update > inserted_key);
  }

  template <typename Key>
  static void was_erased(
    Key& key_to_update,
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


struct ChainArrange {

  // NOTE: You will need to know the size of the container
  // that the keys point to

  template <typename Key>
  static void will_insert(
    Key& key_to_update,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {

  }

  template <typename Key>
  static void was_erased(
    Key& key_to_update,
    const Key& erased_key,
    std::ptrdiff_t count = 1
  ) {

  }
  
};


struct NoShiftArrange {

  template <typename Key>
  static void will_insert(
    Key& key_to_update,
    const Key& inserted_key,
    std::ptrdiff_t count = 1
  ) {

  }

  template <typename Key>
  static void was_erased(
    Key& key_to_update,
    const Key& erased_key,
    std::ptrdiff_t count = 1
  ) {

  }
  
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





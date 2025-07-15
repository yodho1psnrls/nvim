#pragma once

// #include "../utilities.hpp"
#include "../meta_func.hpp"

#include "key_vector.hpp"
#include "key_map.hpp"
#include "iter_key_cont.hpp"

#include <type_traits>
#include <iterator>


template <typename Key, typename Container>
struct to_key_cont {
  static_assert(is_container_v<Container>);
  using type = _if_t<
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<typename Container::iterator>::iterator_category
    >,
    KeyVector<Container, Key>,
    _if_t<
      is_map_container_v<Container>,
      KeyMap<Container>,
      IterKeyCont<Container>
    >
  >;
};


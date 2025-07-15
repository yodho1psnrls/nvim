#pragma once

#include "gtest/gtest.h"

#include "../src/iterators/index_iterator.hpp"
#include "../src/iterators/view.hpp"
#include "../src/containers/key_traits.hpp"

#include <vector>


TEST(KeyTraits, KeyTraitsOnView) {
  IndexIterator ii(69);
  View view(ii, ii+5);
  using Traits = key_cont_traits<decltype(view)>;

  View keys = Traits::keys(view);
  std::array expected = {0, 1, 2, 3, 4};

  EXPECT_EQ(expected, keys);

}


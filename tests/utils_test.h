#pragma once

#include "gtest/gtest.h"

#include "../src/iterators/index_iterator.hpp"
#include "../src/iterators/view.hpp"


TEST(Utilities, RangeToRangeCompareEqualOperator) {
  IndexIterator ii(0);
  View view(ii, ii + 5);
  std::array arr = {0, 1, 2, 3, 4};

  // std::cout << std::boolalpha;
  // std::cout << (view != arr) << "\n";

  EXPECT_TRUE(view == arr);
}

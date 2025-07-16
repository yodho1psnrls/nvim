#pragma once
#include "gtest/gtest.h"

#include "../src/iterators/index_iterator.hpp"
#include "../src/iterators/view.hpp"
#include "../src/containers/arrange_traits.hpp"
#include "../src/containers/key_traits.hpp"
#include "../src/containers/unordered_vector.hpp"
#include "../src/utilities.hpp"

#include <vector>
#include <algorithm>


TEST(ArrangeTraits, ArrangeNormalVectorInsert) {
  IndexIterator ii(0);
  std::vector vec(ii, ii + 7);
  using Traits = arrange_traits<decltype(vec)>;
  using key_type = key_cont_traits<decltype(vec)>::key_type;

  std::vector<key_type> keys = {2, 4};
  const key_type changed(3);

  std::ranges::for_each(keys, [&vec, changed](key_type& k) {
    Traits::will_insert(k, vec, changed, 7);
  });

  std::vector<key_type> expected = {2, 4 + 7};
  EXPECT_EQ(keys, expected);
}


TEST(ArrangeTraits, ArrangeUnorderedVectorInsert) {
  IndexIterator ii('A');
  using vals_type = UnorderedVector<std::vector<int>>;
  vals_type vals(ii, ii + 7);

  using AT = arrange_traits<vals_type>;
  using KT = key_cont_traits<vals_type>;
  using key_type = KT::key_type;

  std::vector<key_type> keys = {2, 6, 3, 4};
  const key_type changed(3);
  const size_t count = 3;

  std::ranges::for_each(keys, [&vals, changed](key_type& k) {
    AT::will_insert(k, vals, changed, count);
  });

  auto it = KT::find(vals, changed);
  KT::insert(vals, it, ii, ii + count);

  // std::vector<key_type> expected = {2, 4, vals.size() - 1};
  // std::vector<key_type> expected = {2, 6, vals.size(), vals.size() + 1};
  std::vector<key_type> expected = {2, 6, vals.size() - count, vals.size() - count + 1};
  EXPECT_EQ(keys, expected);
}


TEST(ArrangeTraits, ArrangeUnorderedVectorErase) {
  IndexIterator ii('A');
  using vals_type = UnorderedVector<std::vector<int>>;
  vals_type vals(ii, ii + 7);

  using AT = arrange_traits<vals_type>;
  using KT = key_cont_traits<vals_type>;
  using key_type = KT::key_type;

  std::vector<key_type> keys = {2, 6, 3, 4, 5};
  const key_type changed(3);
  const size_t count = 2;
  
  // std::vector<key_type> expected = {2, 4, 3, 4, 3};
  std::vector<key_type> expected = keys;
  expected[1] = changed + (count - vals.size() + expected[1]);
  expected[4] = changed + (count - vals.size() + expected[4]);
  
  auto it = KT::find(vals, changed);
  it = KT::erase(vals, it, it + count);

  std::ranges::for_each(keys, [&vals, changed](key_type& k) {
    AT::was_erased(k, vals, changed, count);
  });

  EXPECT_EQ(keys, expected);
}


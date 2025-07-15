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
  ASSERT_EQ(keys, expected);
}


TEST(ArrangeTraits, ArrangeUnorderedVectorInsert) {
  IndexIterator ii('A');
  using vals_type = UnorderedVector<std::vector<int>>;
  vals_type vals(ii, ii + 7);

  using Traits = arrange_traits<vals_type>;
  using key_type = key_cont_traits<vals_type>::key_type;

  std::vector<key_type> keys = {2, 4, 3};
  const key_type changed(3);

  std::ranges::for_each(keys, [&vals, changed](key_type& k) {
    Traits::will_insert(k, vals, changed, 7);
  });

  // auto changed_it = key_cont_traits<vals_type>::find(vals, changed);
  // key_cont_traits<vals_type>::insert(vals, changed_it);

  // std::vector<key_type> expected = {2, 4, vals.size() - 1};
  std::vector<key_type> expected = {2, 4, vals.size()};
  ASSERT_EQ(keys, expected);
}



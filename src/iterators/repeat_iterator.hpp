#pragma once
#include <iterator>

#include "index_iterator.hpp"

// Given an iterator, a distance

template <typename IterType>
class RepeatIterator {
public:

  using iterator_category = typename std::iterator_traits<IterType>::iterator_category;
  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  
  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;

  static_assert(std::is_same_v<iterator_category, std::random_access_iterator_tag>);


private:

  IterType _it;
  
  difference_type _counter; // flat counter value (used for modulo with _dist)
  difference_type _dist; // distance to repeat
 
  // int _counter_until_repeat;
  // int _times_repeated;

};



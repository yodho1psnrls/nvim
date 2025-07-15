#pragma once
#include "../meta_func.hpp" // is_iterator_v
#include <type_traits> // is_move_assignable_v
#include <iterator> // move_iterator, iterator_traits
#include <algorithm> // copy

struct OrderedShift {

  template <typename IterType>
  static void shift_right(
    IterType first,
    IterType last
  ) {
		static_assert(is_iterator_v<IterType>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<IterType>::iterator_category>);
		// std::copy(first, last, std::next(first));
    while(first != last) {
      if constexpr (std::is_move_assignable_v<typename std::iterator_traits<IterType>::value_type>)
        // *std::next(last) = std::move(*last);
        *last = std::move(*std::prev(last));
      else
        // *std::next(last) = *last;
        *last = *std::prev(last);
      --last;
    }
  }
  
  template <typename IterType>
  static void shift_left(
    IterType first,
    IterType last
  ) {
		static_assert(is_iterator_v<IterType>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<IterType>::iterator_category>);
		// std::reverse_copy(first, last, std::prev(first));
    while(first != last) {
      if constexpr (std::is_move_assignable_v<typename std::iterator_traits<IterType>::value_type>)
        *std::prev(first) = std::move(*first);
      else
        *std::prev(first) = *first;
      ++first;
    }
  }

};


struct RotateShift {

  template <typename IterType>
  static void shift_right(
    IterType first,
    IterType last
  ) {
		static_assert(is_iterator_v<IterType>);
    if constexpr (std::is_move_assignable_v<typename std::iterator_traits<IterType>::value_type>)
      *last = std::move(*first);
    else
      *last = *first;
  }
  
  template <typename IterType>
  static void shift_right(
    IterType first,
    IterType last,
    typename std::iterator_traits<IterType>::difference_type n
  ) {
		static_assert(is_iterator_v<IterType>);
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<IterType>::iterator_category>);
		if (n < std::distance(first, last))
			std::copy(first, first + n, last);
			// std::reverse_copy(first, first + n, last);
			// std::copy_backward(first, first + n, last);
		else
			std::copy(first, last, first + n);
  }
  
  template <typename IterType>
  static void shift_left(
    IterType first,
    IterType last
  ) {
		static_assert(is_iterator_v<IterType>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<IterType>::iterator_category>);
    --first;
    --last;
    if constexpr (std::is_move_assignable_v<typename std::iterator_traits<IterType>::value_type>)
      *first = std::move(*last);
    else
      *first = *last;
  }
  
  template <typename IterType>
  static void shift_left(
    IterType first,
    IterType last,
    typename std::iterator_traits<IterType>::difference_type n
  ) {
		static_assert(is_iterator_v<IterType>);
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<IterType>::iterator_category>);
		if (n < std::distance(first, last))
			// std::copy_backward(last, last - n, first - n);
			std::copy(last - n, last, first - n);
		else
			std::copy(first, last, first - n);
  }

};


/*std::vector<int> vec(index_iterator(0), index_iterator(6));
  // RectMatRows rows(3);
  // for(auto it = rows.begin(vec); it != rows.end(vec); ++it)
  //   std::cout << *it << "\n";

  for(auto x : vec)
    std::cout << x << ", ";
  std::cout << "\n";

  // RotateShift::shift_right(vec.begin(), vec.end() - 1);
  // RotateShift::shift_left(vec.begin() + 1, vec.end());
 
  // RotateShift::shift_right(vec.begin(), vec.end() - 3, 3);
  // RotateShift::shift_right(vec.begin(), vec.end() - 3);
  // RotateShift::shift_right(vec.begin() + 1, vec.end() - 2);
  // RotateShift::shift_right(vec.begin() + 2, vec.end() - 1);
  
  RotateShift::shift_left(vec.begin() + 3, vec.end(), 3);*/



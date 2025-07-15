#pragma once

#include "view.hpp"

#include <iterator>


template <typename GenIter>
class GenView {

  GenIter _beg;
  GenIter _end;

public:

  using iterator = GenIter;
  using difference_type = typename std::iterator_traits<GenIter>::difference_type;
  
  using value_type = typename std::iterator_traits<GenIter>::value_type;
  using reference = typename std::iterator_traits<GenIter>::reference;
  using pointer = typename std::iterator_traits<GenIter>::pointer;

  GenView(GenIter end)
  : _beg(end), _end(end) {}
  
  GenView(GenIter begin, GenIter end)
  : _beg(begin), _end(end) {}

  void reconstruct(GenIter end) {
    _beg = end;
    _end = end;
  }
  
  void reconstruct(GenIter begin, GenIter end) {
    _beg = begin;
    _end = end;
  }

  GenIter begin() const { return _beg; }
  GenIter end() const { return _end; }

  bool empty() const { return _beg == _end; }
  bool size() const { return std::distance(_beg, _end); }

  void clear() { _end = _beg; }
  void resize(difference_type n) { _end = _beg; _end += n; }

  GenIter insert(GenIter where, difference_type count = 1) {
    _end += count;
    return where;
  }
  
  GenIter insert(GenIter first, GenIter last) {
    _end += std::distance(first, last);
    return first;
  }
  
  GenIter erase(GenIter where, difference_type count = 1) {
    _end -= count;
    return where;
  }
  
  GenIter erase(GenIter first, GenIter last) {
    _end -= std::distance(first, last);
    return first;
  }

};







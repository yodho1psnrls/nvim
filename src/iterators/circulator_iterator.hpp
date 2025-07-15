#pragma once

#include "../meta_func.hpp" // most_base_t
// #include "../iterators/view.hpp" // View

#include <iterator>


template <typename IterType>
class IteratorToCirculator {

  IterType _it;
  IterType _beg;
  IterType _end;

  template <typename>
  friend class IteratorToCirculator;

public:

  using difference_type = typename std::iterator_traits<IterType>::difference_type;
  using iterator_category = most_base_t<
    std::bidirectional_iterator_tag,
    typename std::iterator_traits<IterType>::iterator_category
  >;

  using value_type = typename std::iterator_traits<IterType>::value_type;
  using reference = typename std::iterator_traits<IterType>::reference;
  using pointer = typename std::iterator_traits<IterType>::pointer;

  IteratorToCirculator(
    const IterType& it,
    const IterType& begin,
    const IterType& end
  ) : _it(it), _beg(begin), _end(end) {}
  
  template <typename IT>
  IteratorToCirculator(const IteratorToCirculator<IT>& other)
  : _it(other._it), _beg(other._beg), _end(other._end) {}


  reference operator*() const { return *_it; }
  pointer operator->() const { return _it.operator->(); }

  bool operator==(const IteratorToCirculator& other) const {
    return _it == other._it;
  }
  
  bool operator!=(const IteratorToCirculator& other) const {
    return _it != other._it;
  }


  IteratorToCirculator& operator++() {
    ++_it;
    if (_it == _end)
      _it = _beg;
    return *this;
  }

  IteratorToCirculator operator++(int) {
    IteratorToCirculator temp(*this);
    ++(*this);
    return temp;
  }
  

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  IteratorToCirculator& operator--() {
    if (_it == _beg)
      _it = _end;
    --_it;
    return *this;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  IteratorToCirculator operator--(int) {
    IteratorToCirculator temp(*this);
    --(*this);
    return temp;
  }


};


template <typename CirType>
class CirculatorToIterator {

  CirType _cir;
  bool _moved;

  // NOTE:
  // other way than using the boolean, but the construction
  //  of this iterator with the boolean variant is seamless
  // int _cycle_num = 0;

public:
  
  using difference_type = typename std::iterator_traits<CirType>::difference_type;
  using iterator_category = most_base_t<
    std::bidirectional_iterator_tag,
    typename std::iterator_traits<CirType>::iterator_category
  >;

  using value_type = typename std::iterator_traits<CirType>::value_type;
  using reference = typename std::iterator_traits<CirType>::reference;
  using pointer = typename std::iterator_traits<CirType>::pointer;

  CirculatorToIterator(const CirType& circulator)
  : _cir(circulator) , _moved(false) {}

  
  reference operator*() const { return *_cir; }
  pointer operator->() const { return _cir.operator->(); }

  bool operator==(const CirculatorToIterator& other) const {
    return _moved && _cir == other._cir;
  }
  
  bool operator!=(const CirculatorToIterator& other) const {
    return _moved && _cir != other._cir;
  }


  CirculatorToIterator& operator++() {
    ++_cir;
    _moved = true;
    return *this;
  }

  CirculatorToIterator operator++(int) {
    CirculatorToIterator temp(*this);
    ++(*this);
    return temp;
  }
  

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  CirculatorToIterator& operator--() {
    --_cir;
    _moved = true;
    return *this;
  }

  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::bidirectional_iterator_tag, IC>>>
  CirculatorToIterator operator--(int) {
    CirculatorToIterator temp(*this);
    --(*this);
    return temp;
  }



};


// TODO:
// template <typename CircType>
// inline auto CirculatorToView(const CircType& circ) {
//  return View(CirculatorToIterator(circ), CirculatorToIterator(circ));
// }




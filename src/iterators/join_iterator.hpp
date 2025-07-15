#pragma once
#include "iterable.hpp"

// Given two ranges, it joins them into a single range
// see boost::join

template <typename IterType>
class JoinIterator {

  IterType _it;

  IterType _jump_from;
  JoinIterator<IterType> _jump_to;

  


public:


};


template <typename IterType>
inline Iterable<JoinIterator<IterType>>
  join(const Iterable<IterType>& lhs, const Iterable<IterType> rhs) {



}


template <typename IterType>
inline Iterable<JoinIterator<IterType>>
  join(const Iterable<JoinIterator<IterType>>& lhs, const Iterable<IterType> rhs) {



}


template <typename IterType>
inline Iterable<JoinIterator<IterType>>
  join(const Iterable<IterType>& lhs, const Iterable<JoinIterator<IterType>> rhs) {



}

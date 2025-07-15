#pragma once
#include "unordered_vector.hpp"
#include "../matrices/matrix.hpp"
#include "../matrices/jagged_matrix.hpp"


/*
template <typename BaseVec>
using UnorderedMatrix = UnorderedVector<Matrix<BaseVec>>;


// A Matrix, which iterable ranges behave like unordered_vector
template <typename BaseVec>
class UnorderedJaggedMatrix 
: public JaggedMatrix<BaseVec> {

  using BaseType = JaggedMatrix<BaseVec>;

public:

  using typename BaseType::const_iterator;
  using typename BaseType::iterator;

  template <typename Container = std::initializer_list<T>>
  iterator insert(const const_iterator& where, const Container& new_row) {
  // void insert(const const_iterator& where, const Container& new_row) {
    int n = new_row.size();
    auto val_it = _vec.insert(where->begin(), new_row.begin(), new_row.end());

    auto id_it = _row_ranges.insert(where.policy().base() + 1, *(where.policy().base()));
    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it += n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }


  template <typename IterType>
  iterator insert(const const_iterator& where, const IterType& first_row_it, const IterType& last_row_it) {
    __flat_iterator<IterType> flat_first(first_row_it);
    __flat_iterator<IterType> flat_last(last_row_it);
    
    auto val_it = _vec.insert(where->begin(), flat_first, flat_last);

    // NOTE: Here you can use boost::transform iterator
    // that transforms the given rows to row.size() + *where->policy().base()
    auto id_it = _row_ranges.insert(where.policy().base() + 1, std::distance(first_row_it, last_row_it), *(where.policy().base()));

    auto iit = id_it;
    difference_type n = 0;
    for(auto it = first_row_it; it != last_row_it; ++it) {
      n += it->size();
      *(iit++) += n;
    }

    while(iit != _row_ranges.end())
      *(iit++) += n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }


  iterator erase(const const_iterator& where) {
    int n = where->size();
    auto val_it = _vec.erase(where->begin(), where->end());
    auto id_it = _row_ranges.erase(where.policy().base() + 1);
    
    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;

    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }

  iterator erase(const const_iterator& first_row_it, const const_iterator& last_row_it) {
    // int n = *(last_row_it.policy().base()) - *(first_row_it.policy().base());
    int n = last_row_it.base() - first_row_it.base();
    
    auto val_it = _vec.erase(first_row_it.base(), last_row_it.base());
    auto id_it = _row_ranges.erase(first_row_it.policy().base() + 1, last_row_it.policy().base() + 1);

    for(auto it = id_it; it != _row_ranges.end(); ++it)
      *it -= n;
    
    return {
      val_it,
      NoPolicy(), 
      typename ranges_vec_type::const_iterator(id_it - 1),
      UseItbSize()
    };
  }


};*/

template <typename VecType>
using UnorderedMatrix = UnorderedVector<Matrix<VecType>>;

template <typename VecType>
using UnorderedJaggedMatrix = UnorderedVector<JaggedMatrix<UnorderedVector<VecType>>>;


template <typename VecType> // Only for vector types !!!
class JaggedMatrix<UnorderedVector<VecType>, true>
: public JaggedMatrix<VecType, true> {

  using BaseType = JaggedMatrix<VecType, true>;
  using ThisType = JaggedMatrix<UnorderedVector<VecType>, true>;

public:

  using BaseType::BaseType;

  using iterator = typename BaseType::iterator;
  using const_iterator = typename BaseType::const_iterator;
  using value_type = typename BaseType::value_type;
  using reference = typename BaseType::reference;
  using const_reference = typename BaseType::const_reference;

  iterator insert(const_iterator where, const value_type& val) {

  }

};







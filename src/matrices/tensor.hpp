#pragma once
#include "../iterators/tensor_iterator.hpp"
#include "../meta_func.hpp"
#include <vector>

template <size_t RANK, typename VecType>
class __Tensor;


// template <size_t RANK, typename VecType>
// using Tensor = 
// _if_t<
//   RANK==0,
//   typename VecType::value_type,
//   _if_t<
//     RANK==1,
//     VecType,
//     __Tensor<RANK, VecType>
//   >
// >;


template <size_t RANK, typename VecType>
struct __tensor_type_impl {
  using type = __Tensor<RANK, VecType>;
};

template <typename VecType>
struct __tensor_type_impl<0, VecType> {
  using type = typename VecType::value_type;
};

// NOTE:
// Dont specialize this, so rank of 1 tensors
//  are stackable and decay to tensor
template <typename VecType>
struct __tensor_type_impl<1, VecType> {
  using type = VecType;
};

// // NOTE:
// // Or just implement those two below
// template <size_t RANK, typename VecType>
// struct __tensor_type_impl<0, __Tensor<RANK, VecType>> {
//   using type = __tensor_type_impl<0 + RANK, VecType>::type;
// };
//
// template <size_t RANK, typename VecType>
// struct __tensor_type_impl<1, __Tensor<RANK, VecType>> {
//   using type = __tensor_type_impl<1 + RANK, VecType>::type;
// };

template <size_t RANK0, size_t RANK1, typename VecType>
struct __tensor_type_impl<RANK0, __Tensor<RANK1, VecType>> {
  using type = __tensor_type_impl<RANK0 + RANK1 - 1, VecType>::type;
};


template <size_t RANK, typename VecType>
using Tensor = __tensor_type_impl<RANK, VecType>::type;




template <size_t RANK, typename VecType>
class __Tensor {
public:

  static_assert(RANK != 0ULL);
  static_assert(is_container_v<VecType>);

  using typename VecType::difference_type;
  using typename VecType::size_type;

private:

  VecType _vec;
  // std::array<std::vector<difference_type>, RANK - 1> _ranges;
  std::array<size_type, RANK - 1> _n;

public:

  using value_type = Tensor<RANK - 1, VecType>;

  // using reference = Iterable


  __Tensor() {};

};




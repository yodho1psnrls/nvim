#pragma once

#include "view.hpp"
#include "jump_iterator.hpp"
#include "crtp_iterator.hpp"

#include <iterator>
#include <type_traits>

// TODO: Implement ViewToIterator Specialization that are out of the regular BaseView types
// , so it is not limited to only IteratorToBaseView inherited View types

template <typename> 
class IteratorToBaseView;

template <
  typename BaseIter,
  typename SizePolicy = FixedJumpPolicy<typename std::iterator_traits<BaseIter>::difference_type>,
  bool STACKABLE = true
>
using BaseTensorView = IteratorToBaseView<stack_jump_iterator_t<BaseIter, SizePolicy, STACKABLE>>;
// using BaseTensorView = _if_t<
//  STACKABLE,
//  IteratorToBaseView<jump_iterator<BaseIter, SizePolicy>>,
//  IteratorToBaseView<rec_jump_iterator_t<jump_iterator<BaseIter, SizePolicy>>>
// >;


template <typename IterType, typename SizePolicy, bool STACKABLE = true>
using InputTensorView = InputViewLayer<BaseTensorView<IterType, SizePolicy, STACKABLE>>;

// Iterable with assignment of same size containers
template <typename IterType, typename SizePolicy, bool STACKABLE = true>
using TensorView = OutputViewLayer<InputViewLayer<BaseTensorView<IterType, SizePolicy, STACKABLE>>>;


template <size_t SKIP_DEPTH = 0>
struct GetAsIterFunctor {
  
  template <typename IterType>
  // const get_base_typedef_t<typename IteratorToBaseView<IterType>::as_iter_type, SKIP_DEPTH>&
  const auto& operator()(const IteratorToBaseView<IterType>& itb) const {
    if constexpr (SKIP_DEPTH == 0)
      return itb.as_iter();
    else if constexpr (SKIP_DEPTH == 1)
      return itb.as_iter().base();
    else
      return get_depth_base<SKIP_DEPTH>(itb.as_iter());
  }
  
  template <typename IterType>
  // get_base_typedef_t<typename IteratorToBaseView<IterType>::as_iter_type, SKIP_DEPTH>&
  auto& operator()(IteratorToBaseView<IterType>& itb) const {
    if constexpr (SKIP_DEPTH == 0)
      return itb.as_iter();
    else if constexpr (SKIP_DEPTH == 1)
      return itb.as_iter().base();
    else
      return get_depth_base<SKIP_DEPTH>(itb.as_iter());
  }

};


// NOTE: More than twice nested ViewToIterator stores additional redundant one byte
// , because of the CrtpIterator's GetIterFunctor which by default is a single std::identity
// , which is optimized by the [[no_unique_adress]] attribute, because its empty
// , but when you nest ViewToIterator
// , it stores multiple differenct instances of it
// , so it has no choise, but to make them addressable, hence - storing one byte per std::identity instance
// 
// EXAMPLE:
// auto vit0 = make_tensor_iterator(make_tensor_view(index_iterator(0), 2));
// auto vit1 = make_tensor_iterator(make_tensor_view(vit0, 3));
// auto vit2 = make_tensor_iterator(make_tensor_view(vit1, 7));
// auto vit3 = make_tensor_iterator(make_tensor_view(vit2, 7));
// auto vit4 = make_tensor_iterator(make_tensor_view(vit3, 7));
// std::cout << sizeof(vit0) << "\n"; // 8
// std::cout << sizeof(vit1) << "\n"; // 16
// std::cout << sizeof(vit2) << "\n"; // 24
// std::cout << sizeof(vit3) << "\n"; // 32
// std::cout << sizeof(vit4) << "\n"; // 40
// WTF: Why sizeof(tensor_iterator) is memory optimized,
//  when there are multiple paths to std::identity in CrtpIterator


template <typename ViewType, size_t SKIP_DEPTH = 0>
class ViewToIterator
: public CrtpIterator<ViewToIterator<ViewType, SKIP_DEPTH>, ViewType, GetAsIterFunctor<SKIP_DEPTH>>{

  // static_assert(!is_iterator_v<ViewType>);
  static_assert(is_view_v<ViewType>);
  // static_assert(std::is_base_of_v<BaseTensorView<typename ViewType::iterator..., ViewType>);

  using ThisType = ViewToIterator<ViewType, SKIP_DEPTH>;
  using CrtpType = CrtpIterator<ThisType, ViewType, GetAsIterFunctor<SKIP_DEPTH>>;
  using IterType = typename CrtpType::template IterType<0>;

protected:

  template <typename, size_t>
  friend class ViewToIterator;

  const ViewType& view() const { return CrtpType::base(); }

public:

  using CrtpType::CrtpType;

  template <typename ITB, typename = std::enable_if_t<std::is_constructible_v<ViewType, ITB>>>
  ViewToIterator(const ViewToIterator<ITB, SKIP_DEPTH>& other)
  // : CrtpType(static_cast<const CrtpIterator<IterableToIterator<ITB>, ITB, GetAsIterFunctor>&>(other).base()) {}
  : CrtpType(other.view()) {}

  // NOTE: !! Very important to overload operator=, because else
  // it would default to using the underlying View type's operator=
  // which will copy all elements, that it refers to, instead of
  // assigning new iterators of the range !!
  //
  // EXAMPLE:
  // std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  // const std::vector<int>& ovec = vec;
  // 
  // auto v = make_tensor_view(vec.begin(), 3);
  // auto vi = make_tensor_iterator(v);
  // vi = make_tensor_iterator(v) + 2;
  //
  // print_cont(vec);
  //
  ThisType& operator=(const ThisType& other) {
    CrtpType::base().reconstruct(other.view());
    return *this;
  }

  // NOTE: The copy constructor of the View Types will only copy the underlying iterators
  // it wont copy the range of elements, as the assignment operator, so we dont need to
  // overide it here, it will default to using the View type's copy constructor, which
  // will only copy the iterators !


  using typename CrtpType::iterator_category;
  using typename CrtpType::difference_type;

  using value_type = ViewType;
  using reference = const value_type&;
  using pointer = const value_type*;

  reference operator*() const { return CrtpType::base(); }
  pointer operator->() const { return &CrtpType::base(); }
  
  // BUG: Why is this working correctly ?
  // Shouldnt this be undefined behaiviour, because you return a reference
  // to a view type which most of its data is stored on the stack in the temp
  // valriable which you create in this scope, and then return a reference
  template <typename IC = iterator_category, typename = std::enable_if_t<
    std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference operator[](difference_type i) const {
    ThisType temp(*this);
    return *(temp += i);
  }


  // static constexpr size_t NUM_DEPTH = IterType::NUM_DEPTH;
  // using depth_base_type = typename IterType::depth_base_type;

  using iterator_equivalent = IterType;

  // using base_type = typename IterType::base_type;
  // base_type& base() { return CrtpType::iter().base(); }
  // const base_type& base() const { return CrtpType::iter().base(); }
  
  template <bool COND = does_have_ref_base_method_v<IterType>,
    typename = std::enable_if_t<COND>>
  auto& base() { return CrtpType::iter().base(); }
  
  template <bool COND = does_have_ref_base_method_v<IterType>,
    typename = std::enable_if_t<COND>>
  const auto& base() const { return CrtpType::iter().base(); }

  template <bool COND = is_jump_iterator_v<IterType>, typename = std::enable_if_t<COND>>
  const auto& policy() const { return CrtpType::iter().policy(); }

};


template <typename ViewType>
ViewToIterator(const ViewType&)
-> ViewToIterator<ViewType>;


template <typename ViewType, size_t SKIP_DEPTH>
struct is_jump_iterator<ViewToIterator<ViewType, SKIP_DEPTH>>
: public is_jump_iterator<typename ViewToIterator<ViewType, SKIP_DEPTH>::iterator_equivalent> {};

template <typename ViewType, size_t SKIP_DEPTH>
struct get_iter_depth<ViewToIterator<ViewType, SKIP_DEPTH>>
: public get_iter_depth<typename ViewToIterator<ViewType, SKIP_DEPTH>::iterator_equivalent> {};

// NOTE: This is not actually used !!!
// template <typename ViewType, size_t SKIP_DEPTH>
// struct rec_jump_iterator<ViewToIterator<ViewType, SKIP_DEPTH>>
// : public rec_jump_iterator<typename ViewToIterator<ViewType, SKIP_DEPTH>::iterator_equivalent> {};

// template <typename ViewType, size_t SKIP_DEPTH>
// struct rec_jump_iterator<ViewToIterator<ViewType, SKIP_DEPTH>> {
//  using type = ViewToIterator<
//    typename ViewType::template changed_iter<
//      rec_jump_iterator_t<
//        typename ViewToIterator<ViewType, SKIP_DEPTH>::iterator_equivalent
//      >
//    , SKIP_DEPTH
//    >
//  >;
// };


struct UseViewSize {};

template <
  typename ViewType,
  typename StridePolicy = UseViewSize,
  bool STACKABLE = true
>
using TensorIterator = _if_t<
  std::is_same_v<StridePolicy, UseViewSize>,
  ViewToIterator<ViewType, 0>,
  stack_jump_iterator_t<ViewToIterator<ViewType, 1>, StridePolicy, STACKABLE>
>;
/*using TensorIterator = _if_t<
  std::is_same_v<StridePolicy, UseViewSize>,
  ViewToIterator<ViewType, 0>,
  _if_t<
    // STCKB && does_have_base_v<typename ViewType::iterator>,
    STACKABLE,
    jump_iterator<ViewToIterator<ViewType, 1>, StridePolicy>,
    rec_jump_iterator_t<jump_iterator<ViewToIterator<ViewType, 1>, StridePolicy>>
  >
>;*/


template <typename IterType>
class IteratorToBaseView {

  static_assert(is_iterator_v<IterType>);
  // static_assert(does_have_ref_base_method_v<IterType>);
  static_assert(std::is_same_v<
    decltype(std::declval<const IterType>().base()),
    const typename IterType::base_type&>
  );

  IterType _it;

protected:

  template <size_t>
  friend class GetAsIterFunctor;
  
  template <typename>
  friend class IteratorToBaseView;

  using as_iter_type = IterType;
  const IterType& as_iter() const noexcept { return _it; }
  IterType& as_iter() noexcept { return _it; }

public:

  // template <typename IT>
  // using changed_iter = IteratorToBaseView<IT>;
  
  using iterator = typename IterType::base_type;
  using difference_type = typename std::iterator_traits<iterator>::difference_type;
 
  template <typename ...Args, typename = std::enable_if_t<
    std::is_constructible_v<IterType, Args...>>>
  explicit IteratorToBaseView(Args&&... args)
  : _it(std::forward<Args>(args)...) {}

  template <typename IT, typename = std::enable_if_t<std::is_constructible_v<IterType, IT>>>
  IteratorToBaseView(const IteratorToBaseView<IT>& other)
  : _it(other.as_iter()) {}
  
  // NOTE: TESTING !!!
 //  IteratorToBaseView(const IteratorToBaseView& other)
  // : _it(other._it) {
  //  std::cout << "slqlqlql\n";
  // }

  const iterator& begin() const {
    return _it.base();
  }
  
  iterator end() const {
    IterType temp(_it);
    return (++temp).base();
  }

  // difference_type size() const { return std::distance(begin(), end()); }
  // bool empty() const { return begin() == end(); }

  difference_type size() const {
    // if constexpr (STACKABLE)
    //  return _it.stride();
    // else
      return std::distance(begin(), end());
  }

  bool empty() const {
    // if constexpr (STACKABLE)
    //  return !_it.stride();
    // else
      return begin() == end();
  }

  template <typename IT>
  void reconstruct(const IteratorToBaseView<IT>& other) {
    // static_assert(std::is_convertible_v<IT, IterType>);
    // static_assert(std::is_assignable_v<IT, IterType>);
    _it = other.as_iter();
  }

  template <typename ...Args, typename =
    std::enable_if_t<std::is_constructible_v<IterType, Args...>>>
  void reconstruct(Args&&... args) {
    _it = IterType(std::forward<Args>(args)...);
  }

};


template <typename BaseIter, typename SizePolicyArg>
IteratorToBaseView(const BaseIter&, SizePolicyArg&&)
// IteratorToBaseView(const BaseIter&, const SizePolicyArg&)
-> IteratorToBaseView<JumpIterator<BaseIter, arg_to_jump_policy_t<SizePolicyArg>>>;


// ============================ HELPER FUNCTIONS ============================== //


template <bool STACKABLE = true, typename IterType, typename SizeArg>
inline auto make_tensor_view(const IterType& first, SizeArg&& size) {
  return TensorView<IterType, arg_to_jump_policy_t<SizeArg>, STACKABLE>(
    first, std::forward<SizeArg>(size)
  );
}

// template <typename ViewType>
template <bool STACKABLE = true, typename ViewType>
ViewToIterator<ViewType, 0>
inline make_tensor_iterator(const ViewType& first) {
  static_assert(is_view_v<ViewType>);
  return ViewToIterator<ViewType, 0>(first);
}

// template <typename ViewType>
template <bool STACKABLE = true, typename ViewType>
ViewToIterator<ViewType, 0>
inline make_tensor_iterator(const ViewType& first, const UseViewSize&) {
  static_assert(is_view_v<ViewType>);
  return ViewToIterator<ViewType, 0>(first);
}

template <bool STACKABLE = true, typename ViewType, typename StrideArg,
  typename = std::enable_if_t<is_view_v<ViewType>>>
inline auto make_tensor_iterator(const ViewType& first, StrideArg&& stride) {
  // static_assert(is_view_v<ViewType>);
  static_assert(!std::is_same_v<UseViewSize, StrideArg>);

  return TensorIterator<ViewType, arg_to_jump_policy_t<StrideArg>, STACKABLE>(
    ViewToIterator<ViewType, 1>(first), std::forward<StrideArg>(stride)
  );
}

// FIX:
template <bool STACKABLE = true, typename IterType, typename SizeArg, typename StrideArg = UseViewSize,
 typename = std::enable_if_t<is_iterator_v<IterType>>>
inline auto make_tensor_iterator(const IterType& first, SizeArg&& size, StrideArg&& stride = UseViewSize()) {
  if constexpr (std::is_same_v<arg_to_jump_policy_t<StrideArg>, UseViewSize>)
    return make_tensor_iterator<STACKABLE>(
      make_tensor_view<STACKABLE>(
        first,
        std::forward<SizeArg>(size)
      ));
  else
    return make_tensor_iterator<STACKABLE>(
      make_tensor_view<STACKABLE>(
        first,
        std::forward<SizeArg>(size)
      ),
      std::forward<StrideArg>(stride));
}

// Overload for TensorIterator to give directly the underlying iterator
// because we are sure that the underlying range is contiguous
// BUG: With CycleMatrix's TensorIterator
// (After all, thats what TensorIterator wraps over)
// template <typename ViewType, typename Policy, bool STACKABLE>
// inline typename ViewType::iterator make_flat_iterator(
//  const TensorIterator<ViewType, Policy, STACKABLE>& iter
// ) {
//  return iter->begin();
// }





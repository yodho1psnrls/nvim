#pragma once

#include "kernel_fuse.hpp"
// #include "kernel_impl.hpp"

// NOTE: We need a KernelHolder helper class, so we can inherit it before
// the interfaces, so its constructor is called before the interfaces constructors

// template <typename ...TaggedContainers>
// struct KernelHolder {
//   using kernel_type = KeyKernelImpl<TaggedContainers...>;
//   kernel_type _ker;
// };

template <typename KernelType>
struct KernelHolder {
  using kernel_type = KernelType;
  KernelType _ker;
};


template <typename T>
struct kernel_type_result;

template <typename T>
using kernel_type_result_t = typename kernel_type_result<T>::type;


// NOTE: With this mixin you can create new classes by just using a simple type alias,
// but if you have Interfaces with more than one typename template parameter, then
// you would have to define it directly, without using this Mixin helper
// For more info search for Curious Mixin (CRTP Mixin), or FluentC++'s blog post about Variadic CRTP

template <
  typename KernelType,
  template <typename> class ...Interfaces
>
struct KernelMixin
// : public KernelHolder<KernelType>
: public KernelHolder<kernel_type_result_t<KernelMixin<KernelType, Interfaces...>>>
, public Interfaces<KernelMixin<KernelType, Interfaces...>>... {};


// A class for the types like Mesh and PointClouds to specialize
// for the wanted value types they want to include in the Kernel data
// template <typename Type>
// struct kernel_traits;

template <template <typename...> class Interface, typename KernelType>
struct kernel_interface_traits;


template <typename KernelType, template <typename> class ...Interfaces>
struct kernel_type_result<KernelMixin<KernelType, Interfaces...>>
: public kernel_insert_value_type_lists<
    KernelType,
    // typename Interfaces<KernelMixin<KernelType, Interfaces...>>::inserts... // This causes instantiation loop, which ends up in not finding the type alias here
    // typename kernel_traits<Interfaces<KernelMixin<KernelType, Interfaces...>>>::inserts...
    typename kernel_interface_traits<Interfaces, KernelType>::inserts...
  > {};
  
// TEST:
// using kernel_type = kernel_insert_value_type_lists_t<KeyKernelImpl<>, type_list<TaggedType<float, vert_handle_tag>>>;
// using kernel_type = KeyKernelImpl<>;
// static_assert(std::is_same_v<
//   kernel_type,
//   // kernel_type_result_t<KernelMixin<kernel_type>>
//   kernel_type_result_t<KernelMixin<kernel_type, PointCloudInterface>>
// >);

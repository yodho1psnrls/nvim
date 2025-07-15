#pragma once

#include "../iterators/view.hpp"
#include "../keys.hpp"
// #include "../keys/kernel_fuse.hpp"
#include "../meta_func.hpp"
#include "../kernel/key_kernel.hpp"

#include <vector>


template <typename BaseType>
class PointCloudLayer;


// template <typename BaseType>
// struct kernel_traits<PointCloudInterface<BaseType>> {
//   // Type list of TaggedValues
//   using inserts = type_list<>;  // New values that the interface introduces into the Kernel
// };

template<typename KernelType>
struct kernel_interface_traits<PointCloudLayer, KernelType> {
  // Type list of TaggedValues
  using inserts = type_list<>;  // New values that the interface introduces into the Kernel
};


template <typename Derived>  // NOTE: Meant for CRTP
class PointCloudLayer {
public:

  // using kernel_type = typename BaseType::kernel_type;
  using kernel_type = kernel_type_result_t<Derived>;
  static_assert(tuple_contains_type_v<vert_handle_tag, typename kernel_type::tag_types>);

private:

  kernel_type& kernel() { return static_cast<Derived*>(this)->_ker; }
  const kernel_type& kernel() const { return static_cast<const Derived*>(this)->_ker; }

// protected:
public:

  // Type list of TaggedValues
  // using inserts = type_list<>;  // New values that the interface introduces into the Kernel
  // using inserts = type_list<TaggedType<int, vert_handle_tag>>; // For Test
  // using expects = type_list<>;  // Required values already included into the Kernel

public:

  using vert_key = kernel_type::template tag_to_key_type<vert_handle_tag>;
  using vert_key_iter = kernel_type::template tag_to_key_iterator<vert_handle_tag>;
  using vert_value_type = kernel_type::template tag_to_value_type<vert_handle_tag>;
  using vert_reference = kernel_type::template tag_to_reference<vert_handle_tag>;
  using vert_const_reference = kernel_type::template tag_to_const_reference<vert_handle_tag>;

  vert_key_iter verts_begin() const { return kernel().template begin<vert_handle_tag>(); }
  vert_key_iter verts_end() const { return kernel().template end<vert_handle_tag>(); }

  View<vert_key_iter> verts() const {
    return View(verts_begin(), verts_end());
  }

  size_t verts_size() const {
    return kernel().template size<vert_handle_tag>();
  }

  vert_reference vert(vert_key vi) {
    return kernel().at(vi);
  }
  
  vert_const_reference vert(vert_key vi) const {
    return kernel().at(vi);
  }

  vert_key add_vert(const vert_value_type& v) {
    return *(kernel().insert(verts_end(), v));
  }

  void remove_vert(vert_key vi) {
    kernel().erase(kernel().find(vi));
  }

};


/*template <typename Derived>  // NOTE: Meant for CRTP
class PointCloudInterface {
private:

  auto& kernel() { return static_cast<Derived*>(this)->kernel(); }
  const auto& kernel() const { return static_cast<const Derived*>(this)->kernel(); }
  // auto& kernel() { return static_cast<Derived&>(*this).kernel(); }
  // const auto& kernel() const { return static_cast<const Derived&>(*this).kernel(); }

public:

  auto verts_begin() const { return kernel().template begin<vert_handle_tag>(); }
  auto verts_end() const { return kernel().template end<vert_handle_tag>(); }

  auto verts() const {
    return View(verts_begin(), verts_end());
  }

  auto vert(auto vi) {
    return kernel().at(vi);
  }
  
  auto vert(auto vi) const {
    return kernel().at(vi);
  }

  auto add_vert(const auto& v) {
    return *(kernel().insert(verts_end(), v));
  }

  void remove_vert(auto vi) {
    kernel().erase(kernel().find(vi));
  }

};*/


/*template <typename VertType>
struct PointCloud
: public KernelHolder<TaggedType<std::vector<VertType>, vert_handle_tag>>
, public PointCloudInterface<PointCloud<VertType>> {

  using kernel_type = KeyKernelImpl<TaggedType<std::vector<VertType>, vert_handle_tag>>;
  using ThisType = PointCloud<VertType>;
  using BaseType = PointCloudInterface<ThisType>;

};*/


template <typename VertType>
using PointCloud = KernelMixin<
  // KeyKernelImpl<TaggedType<std::vector<VertType>, vert_handle_tag>>,
  kernel_insert_value_types_t<KeyKernelImpl<>, TaggedType<VertType, vert_handle_tag>>,
  PointCloudLayer
>;




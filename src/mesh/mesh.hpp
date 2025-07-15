#pragma once

#include "../keys.hpp"
#include "../kernel/key_kernel.hpp"
#include "../point_cloud/point_cloud.hpp"

// NOTE: Mesh just has to define which key-value pairs it needs
// then using the fuse_traits, you include them into the Kernel Type

// template <typename Cont, typename Tag>
// using get_key_type_t = TaggedType<typename key_cont_traits<Cont>::key_type, Tag>;

// template <typename Kernel = KeyKernelImpl<>>
// template <typename BaseType = KernelCore>
// class MeshLayer
// : public BaseType {

template <typename BaseType>
class MeshLayer;

template <typename KernelType>
struct kernel_interface_traits<MeshLayer, KernelType> {
  // Type list of TaggedValues
  // New values that the interface introduces into the Kernel
  using inserts = type_list<
    TaggedType<typename KernelType::template tag_to_key_type<vert_handle_tag>, hedge_handle_tag>,
    // TaggedType<std::array<typename KernelType::template tag_to_key_type<hedge_handle_tag>, 3>, face_handle_tag>
    TaggedType<std::array<hedge_handle_tag, 3>, face_handle_tag>
  >;
};

template <typename Derived>
class MeshLayer {

public:

  // NOTE: Fused with the necessary types
  // using kernel_type = kernel_insert_t<
  //  typename KernelCore::kernel_type,
  //  TaggedType<Vertex, vert_handle_tag>,
  //  TaggedType<get_key_type_t<, vert_handle_tag>,
  // >;
  
  // using kernel_type = typename BaseType::kernel_type;
  using kernel_type = kernel_type_result_t<Derived>;
  static_assert(tuple_contains_type_v<vert_handle_tag, typename kernel_type::tag_types>);
  static_assert(tuple_contains_type_v<face_handle_tag, typename kernel_type::tag_types>);

private:

  kernel_type& kernel() { return static_cast<Derived*>(this)->_ker; }
  const kernel_type& kernel() const { return static_cast<const Derived*>(this)->_ker; }
  
  using vert_key = kernel_type::template tag_to_key_type<vert_handle_tag>;

  // auto get_lambda = [*this]<typename FromKey, typename ToKey>(FromKey fk){return this->}

// public:
//   using inserts = type_list<TaggedType<vert_key, face_handle_tag>>;

public:

  using face_key = kernel_type::template tag_to_key_type<face_handle_tag>;
  using face_key_iter = kernel_type::template tag_to_key_iterator<face_handle_tag>;
  using face_value_type = kernel_type::template tag_to_value_type<face_handle_tag>;
  using face_reference = kernel_type::template tag_to_reference<face_handle_tag>;
  using face_const_reference = kernel_type::template tag_to_const_reference<face_handle_tag>;

  using hedge_key = kernel_type::template tag_to_key_type<hedge_handle_tag>;
  using hedge_key_iter = kernel_type::template tag_to_key_iterator<hedge_handle_tag>;
  using hedge_value_type = kernel_type::template tag_to_value_type<hedge_handle_tag>;
  using hedge_reference = kernel_type::template tag_to_reference<hedge_handle_tag>;
  using hedge_const_reference = kernel_type::template tag_to_const_reference<hedge_handle_tag>;


  face_key_iter hedges_begin() const { return kernel().template begin<hedge_handle_tag>(); }
  face_key_iter hedges_end() const { return kernel().template end<hedge_handle_tag>(); }

  View<hedge_key_iter> hedges() const {
    return View(hedges_begin(), hedges_end());
  }
  
  size_t hedges_size() const {
    return kernel().template size<hedge_handle_tag>();
  }
  
  hedge_reference hedge(hedge_key he) {
    return kernel().at(he);
  }
  
  hedge_const_reference hedge(hedge_key he) const {
    return kernel().at(he);
  }

  // Returns a list of all half-edge keys of the face
  const std::array<hedge_key, 3>& hedges(face_key f) const {
    return std::get<std::array<hedge_key, 3>>(face(f));
  }
  
  // Returns a list of all vertex keys of the face
  const auto& verts(face_key f) const {
    // return std::get<vert_key>(hedge(std::get<std::array<hedge_key, 3>>(face(f))));
    auto& hs = hedges(f);
    // FIX:
    return View(make_lookup_iterator(hs.begin(), *this), make_lookup_iterator(hs.end(), *this));
    // return View(TransformIterator(hs.begin(), get_lambda), make_lookup_iterator(hs.end(), get_lambda));
  }


  face_key_iter faces_begin() const { return kernel().template begin<face_handle_tag>(); }
  face_key_iter faces_end() const { return kernel().template end<face_handle_tag>(); }

  View<face_key_iter> faces() const {
    return View(faces_begin(), faces_end());
  }

  size_t faces_size() const {
    return kernel().template size<face_handle_tag>();
  }

  face_reference face(face_key f) {
    return kernel().at(f);
  }
  
  face_const_reference face(face_key f) const {
    return kernel().at(f);
  }

  face_key add_face(const face_value_type& v) {
    return *(kernel().insert(faces_end(), v));
  }

  void remove_face(face_key vi) {
    kernel().erase(kernel().find(vi));
  }

};



// template <typename VertType, typename FaceType>
// class Mesh
// : public KernelHolder<KeyKernelImpl<
//     TaggedType<std::vector<VertType>, vert_handle_tag>,
//     TaggedType<std::vector<FaceType>, face_handle_tag>
//   >>
// , public PointCloudLayer<Mesh<VertType, FaceType>>
// , public MeshLayer<Mesh<VertType, FaceType>> {};



template <typename VertType, typename FaceType>
struct __get_mesh_kernel_type {
  using type = kernel_insert_value_types_t<
    KeyKernelImpl<>,
    TaggedType<VertType, vert_handle_tag>,
    TaggedType<FaceType, face_handle_tag>
  >;
};

template <typename VertType>
struct __get_mesh_kernel_type<VertType, void> {
  using type = kernel_insert_value_types_t<
    KeyKernelImpl<>,
    TaggedType<VertType, vert_handle_tag>
  >;
};

template <typename VertType, typename FaceType>
using __get_mesh_kernel_type_t = typename __get_mesh_kernel_type<VertType, FaceType>::type;


template <typename VertType, typename FaceType = void>
using Mesh = KernelMixin<
  __get_mesh_kernel_type_t<VertType, FaceType>,
  // _if_t<
  //   std::is_same_v<FaceType, void>,
  //   kernel_insert_value_types_t<
  //     KeyKernelImpl<>,
  //     TaggedType<VertType, vert_handle_tag>
  //   >,
  //   kernel_insert_value_types_t<
  //     KeyKernelImpl<>,
  //     TaggedType<VertType, vert_handle_tag>,
  //     TaggedType<FaceType, face_handle_tag>
  //   >
  // >,
  PointCloudLayer,
  MeshLayer
>;





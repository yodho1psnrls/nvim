#pragma once

#include "../containers/property_container.hpp"
#include "../tagged.hpp"
#include "../meta_func.hpp"


// NOTE:
// The default base class for all meshes and graphs
// serves as the absolute base, which holds the KeyKernel
// From there on, in the hierarchy each class either inheris
// from other mesh, point cloud or graph class or if not, it
// inherits from this
// This is helpful, because we can have a lot of hierarchies, that
// only add functionality, without storing any data, which is contained
// only here !!
/*template <typename KernelType = KeyKernelImpl<>>
class KernelCore {

  KernelType _ker;

protected:

  using kernel_type = KeyKernelImpl<>;
  kernel_type& kernel() { return _ker; }
  const kernel_type& kernel() const { return _ker; }

};*/


// KeyKernelImpl<TaggedType<PropertyContainer<...>, Tag>>


/*template <typename KernelType, typename TaggedCont,
  // bool = KernelType::template contains_tag<typename TaggedCont::tag_type>>
  bool = tuple_contains_type_v<typename TaggedCont::tag_type, typename KernelType::tags_list>>
struct kernel_insert;

template <typename KernelType, typename Cont, typename Tag>
class kernel_insert<KernelType, TaggedType<Cont, Tag>, true> {
  using cont_type = typename KernelType::template tag_to_cont_type<Tag>;
public:
  using type = tuple_replace_t<
    TaggedType<cont_type, Tag>,
    TaggedType<push_back_type_t<cont_type, Cont>, Tag>,
    KernelType
  >;
};

template <typename KernelType, typename Cont, typename Tag>
struct kernel_insert<KernelType, TaggedType<Cont, Tag>, false> {
  // using type = push_back_type_t<KernelType, TaggedType<Cont, Tag>>;
  using type = push_back_type_t<KernelType, TaggedType<PropertyContainer<Cont>, Tag>>;
};

template <typename KernelType, typename TagType>
using kernel_insert_t = typename kernel_insert<KernelType, TagType>::type;*/
  
// TEST:
// using test0 = kernel_insert_t<KeyKernelImpl<>, TaggedType<std::vector<char>, vert_handle_tag>>;
// using test1 = kernel_insert_t<test0, TaggedType<std::vector<std::string>, vert_handle_tag>>;
// using test2 = kernel_insert_t<test1, TaggedType<Matrix<std::vector<int>>, face_handle_tag>>;
// using result = KeyKernelImpl<
//  TaggedType<PropertyContainer<
//    std::vector<char>,
//    std::vector<std::string>
//  >, vert_handle_tag>,
//  TaggedType<PropertyContainer<
//    Matrix<std::vector<int>>
//  >, face_handle_tag>
// >;
// static_assert(std::is_same_v<result, test2>);

template <
  typename T,
  typename KernelType,
  bool is_tag = tuple_contains_type_v<T, typename KernelType::tag_types>
>
struct __get_key_if_tag;

template <typename T, typename KernelType>
struct __get_key_if_tag<T, KernelType, false> {
  using type = T;
};

template <typename T, typename KernelType>
struct __get_key_if_tag<T, KernelType, true> {
  using type = typename KernelType::template tag_to_key_type<T>;
};

template <typename T, typename KernelType>
using __get_key_if_tag_t = typename __get_key_if_tag<T, KernelType>::type;



template <typename KernelType, typename TaggedValue,
  // bool = KernelType::template contains_tag<typename TaggedValue::tag_type>>
  bool = tuple_contains_type_v<typename TaggedValue::tag_type, typename KernelType::tag_types>>
struct __kernel_insert_one;

template <typename KernelType, typename Value, typename Tag>
class __kernel_insert_one<KernelType, TaggedType<Value, Tag>, true> {
  using cont_type = typename KernelType::template tag_to_cont_type<Tag>;
public:
  using type = tuple_replace_t<
    TaggedType<cont_type, Tag>,
    TaggedType<
      push_back_type_t<cont_type, std::vector<
        __get_key_if_tag<Value, KernelType>
      >>, Tag
    >,
    KernelType
  >;
};

template <typename KernelType, typename Value, typename Tag>
struct __kernel_insert_one<KernelType, TaggedType<Value, Tag>, false> {
  using type = push_back_type_t<KernelType, TaggedType<PropertyContainer<std::vector<Value>>, Tag>>;
};

template <typename KernelType, typename TaggedValue>
using __kernel_insert_one_t = typename __kernel_insert_one<KernelType, TaggedValue>::type;


template <typename KernelType, typename ...TaggedValues>
struct kernel_insert_value_types;

template <typename KernelType>
struct kernel_insert_value_types<KernelType> {
  using type = KernelType;
};

template <typename KernelType, typename TaggedValue, typename ...Rest>
struct kernel_insert_value_types<KernelType, TaggedValue, Rest...>
: public kernel_insert_value_types<__kernel_insert_one_t<KernelType, TaggedValue>, Rest...> {};

template <typename KernelType, typename ...TaggedValues>
using kernel_insert_value_types_t = typename kernel_insert_value_types<KernelType, TaggedValues...>::type;


template <typename KernelType, typename ...ValueTypeLists>
struct kernel_insert_value_type_lists;

template <typename KernelType>
struct kernel_insert_value_type_lists<KernelType> {
  using type = KernelType;
};

template <typename KernelType, typename ...TaggedValues, typename ...RestLists>
struct kernel_insert_value_type_lists<KernelType, type_list<TaggedValues...>, RestLists...>
: public kernel_insert_value_type_lists<kernel_insert_value_types_t<KernelType, TaggedValues...>, RestLists...> {};

template <typename KernelType, typename ...ValueTypeLists>
using kernel_insert_value_type_lists_t = typename kernel_insert_value_type_lists<KernelType, ValueTypeLists...>::type;



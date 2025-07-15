#pragma once
#include "gtest/gtest.h"

#include "../src/keys.hpp"
#include "../src/kernel/kernel_impl.hpp"
// #include "../src/kernel/kernel_fuse.hpp"
#include "../src/matrices/jagged_matrix.hpp"

#include "../src/iterators/zip_iterator.hpp"
#include "../src/containers/property_container.hpp"

#include <vector>


TEST(Kernel, KernelAreKeysUpdatedOnErase) {
  
  using vert_key = TaggedType<size_t, vert_handle_tag>;
  using hedge_key = TaggedType<size_t, hedge_handle_tag>;
  using face_key = TaggedType<size_t, face_handle_tag>;

  using vert_key_iter = TaggedIterator<IndexIterator<size_t>, vert_handle_tag>;
  using hedge_key_iter = TaggedIterator<IndexIterator<size_t>, hedge_handle_tag>;

  auto vkit = ZipIterator(vert_key_iter(0ULL));

  // For a Matrix
  using kernel_type = KeyKernelImpl<
    TaggedType<PropertyContainer<
      std::vector<char>
    >, vert_handle_tag>,
    TaggedType<PropertyContainer<
      std::vector<vert_key>
    >, hedge_handle_tag>
    // TaggedType<PropertyContainer<
    //   std::vector<View<hedge_key_iter>>
    // >, face_handle_tag>
  >;

  kernel_type ker;

  ZipIterator zit(IndexIterator('A'));

  // ker.insert(ker.end<vert_handle_tag>(), 13, 'J');
  ker.insert(ker.end<vert_handle_tag>(), zit, zit + 13);
  ker.insert(ker.end<hedge_handle_tag>(), vkit + 2, vkit + 9);
  
  View vals = ker.values<hedge_key, vert_key>();
  std::array<size_t, 7> expected;
  
  expected = {2, 3, 4, 5, 6, 7, 8};
  EXPECT_EQ(expected, vals);
  
  // for(auto& x : ker.values<hedge_key, vert_key>())
  //   std::cout << x << ", ";
  // std::cout << "\n";

  auto vit = ker.begin<vert_handle_tag>() + 4;
  // ker.insert(vit, 7, {'x'});
  // ker.insert(vit, zit+13, zit+20);
  // ker.erase(vit, vit + 7);

  // ker.erase(ker.insert(vit, {'J'}));
  vit = ker.insert(vit, 7, {'J'});

  expected = {2, 3, 4, 12, 13, 14, 15};
  EXPECT_EQ(expected, vals);
  
  vit = ker.erase(vit, vit + 7);
  
  expected = {2, 3, 4, 5, 6, 7, 8};
  EXPECT_EQ(expected, vals);
  
  // for(face_key x : ker.keys<face_key>())
  //   std::cout << x << ", ";
  // std::cout << "\n";

}


TEST(Kernel, KernelAreKeysUpdatedWithMatrix) {
  // EXPECT_EQ(5, 5);

  using vert_cont = PropertyContainer<std::vector<char>>;
  using vert_key = TaggedType<
    key_cont_traits<vert_cont>::key_type,
    vert_handle_tag
  >;

  using hedge_cont = PropertyContainer<std::vector<vert_key>>;
  using hedge_key = TaggedType<
    typename key_cont_traits<hedge_cont>::key_type,
    hedge_handle_tag
  >;

  using face_cont = PropertyContainer<JaggedMatrix<std::vector<vert_key>>>;
  using face_key = TaggedType<
    key_cont_traits<face_cont>::key_type,
    face_handle_tag
  >;

  // using hedge_cont = View<

  using ker_type = KeyKernelImpl<
    TaggedType<vert_cont, vert_handle_tag>,
    // TaggedType<hedge_cont, vert_handle_tag>,
    TaggedType<face_cont, face_handle_tag>
  >;

  ker_type ker;

  ZipIterator vi(IndexIterator('A'));
  // ZipIterator vii(TensorIterator(TransformIterator(
  //   IndexIterator(0LL),
  //   [](size_t x){ return vert_key(x); }
  // )), 3);

  ker.insert(ker.end<vert_key>(), vi, vi+6);
  // ker.insert(ker.end<face_key>(), View(vii, vii+6));
  // ker.insert(ker.end<face_key>(), {{vert_key(0), vert_key(1), vert_key(2)}});
  // ker.insert(ker.end<face_key>(), {{vert_key(3), vert_key(4), vert_key(5)}});
  ker.push_back<face_key>({{vert_key(0), vert_key(1), vert_key(2)}});
  ker.push_back<face_key>({{vert_key(3), vert_key(4), vert_key(5)}});


  View vals = ker.values<face_key, std::vector<vert_key>>();
  std::array<std::array<size_t, 3>, 2> expected({{0, 1, 2}, {3, 4, 5}});
  EXPECT_EQ(expected, vals);
  
  // print_cont(ker.values<face_key, std::vector<vert_key>>());
  // print_cont(ker.values<face_key>());
  // print_cont(ker.values<face_key, std::vector<vert_key>>());
}


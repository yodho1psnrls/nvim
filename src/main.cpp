#include "utilities.hpp"
#include "iterators/view.hpp"
#include "iterators/index_iterator.hpp"
#include "containers/key_traits.hpp"
#include "iterators/tensor_iterator.hpp"

#include <iostream>
#include <vector>
#include <tuple>
#include <array>


int main() {

  IndexIterator ii(0);
  size_t n = 3;

  std::vector vec(ii + 17, ii + n + 17);
  auto tup = std::forward_as_tuple(vec);

  auto view = make_tensor_view(ii, n);
  // vec = view;
  tup = std::make_tuple(view);

  print_cont(vec);



  return 0;
}

#include <iostream>
#include "iterators/view.hpp"
#include "iterators/index_iterator.hpp"
#include <array>
#include <vector>

int main() {

  IndexIterator ii(0);
  View view(ii, ii + 5);
  std::array arr = {0, 1, 2, 3, 4};

  std::cout << std::boolalpha;
  std::cout << (view != arr) << "\n";


  return 0;
}

#pragma once

#include <memory>

// https://www.geeksforgeeks.org/resize-vector-without-initializing-new-elements-in-cpp/


// An allocator that makes the stl containers
//  to not call the constructor on resize()
template <typename Alloc>
// template <typename T>
class NoInitAlloc {

  [[no_unique_address]] Alloc _alloc;

public:

  using value_type = typename std::allocator_traits<Alloc>::value_type;
  using size_type = typename std::allocator_traits<Alloc>::size_type;
  using pointer = typename std::allocator_traits<Alloc>::pointer;
  // using value_type = T;
  
  // Allocate sufficient memory
  value_type* allocate(size_type n) {
      // return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    return std::allocator_traits<Alloc>::allocate(_alloc, n);
  }
  
  void deallocate(value_type* p, size_type n) noexcept {
      // ::operator delete(p, sizeof(value_type) * n);
    return std::allocator_traits<Alloc>::deallocate(_alloc, p, n);
  }

  // Override construct to skip initialization
  template <typename U, typename... Args>
  void construct(U* p, Args&&...) noexcept {
      // No initialization occurs
  }

  template <typename U>
  void destroy(U* p) noexcept {
    return std::allocator_traits<Alloc>::destroy(_alloc, p);
  }

};


// BUG: NOO, this will use the methods of the base Alloc
// NOTE: Specialization to inherit all the typedefs
// , because we need the same typedefs in NoInitAlloc
// template <typename Alloc>
// struct std::allocator_traits<NoInitAlloc<Alloc>>
// : public std::allocator_traits<Alloc> {};




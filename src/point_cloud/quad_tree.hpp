#pragma once
#include "point_vec.hpp"

// References:
// https://stackoverflow.com/questions/29145859/advice-on-data-structure-for-point-clouds
// https://geidav.wordpress.com/2014/07/18/advanced-octrees-1-preliminaries-insertion-strategies-and-max-tree-depth/

namespace pc {


template <typename V>
class QuadTree : public PointVec<V> {

  using PointVec<V>::_vertices;
  


};



} // namespace pc



#pragma once

// #include "../../olc/olcPixelGameEngine.h" // glm::uvec2

#include "point_vec.hpp"
#include "../key_cont/adaptors/keys.hpp"                     // index_vert_handle
#include "../iterators/index_iterator.hpp"
#include "../utilities.hpp"               // uint

#include <vector>
#include <cassert>
#include <stdexcept>
#include <iterator>

#include <limits>
#include <queue>

#include <cstdlib>
#include <iostream>

#include "../sdf/sdfs.h"
#include "glm/glm.hpp"


// References:
// https://carmencincotti.com/2022-10-31/spatial-hash-maps-part-one/
// https://www.youtube.com/watch?v=D2M8jTtKi44 (dense spatial hash table)
// https://matthias-research.github.io/pages/tenMinutePhysics/index.html
// https://matthias-research.github.io/pages/tenMinutePhysics/11-hashing.pdf
// https://en.wikipedia.org/wiki/Z-order_curve (consider trying this in the future and profile the difference)
// https://gamedev.stackexchange.com/questions/44278/space-partitioning-when-everything-is-moving
// https://handmade.network/p/75/monter/blog/p/2978-collision_system_part_2__sparse_hash-based_grid_partitioning%252C_multithreading%252C_and_future
// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/spatial-hashing-r2697/
// https://docs.taichi-lang.org/docs/sparse

// NOTE: This stores only infinitely small points.
// If you want to store objects with a size in the future:
//  1) You would need to add that object to all buckets that it
//  overlaps with and when you iterate all points, you would
//  need to mark visited vertices, so you dont visit them twice.
//  2) Or you would need to store the minpoint of the object
//  , but then also store the all buckets that the object overlaps with.

// Typically in a hash table, we are allowed only to add/insert and remove/erase
//  elements and not change them, but here we will give the power to the user to
//  change the attributes of the stored vertices, and provide a method update_hash
//  or something similar, that will check all vertices positions if they are in
//  the correct bucket, and if not, it moves it to the correct bucket
// Also provide a boolean method is_hashed or are_correct_buckets, that checks if
//  update_hash was performed


namespace pc {


// Expects that V has a glm::vec2 pos attribute
template <typename V>
class HashGrid : public PointVec<V> {
public:

  using vert_type = typename PointVec<V>::vert_type;
  using vert_container = HashGrid<V>;

  using difference_type = std::ptrdiff_t;
  using size_type = size_t;

  using vert_key = index_vert_handle<difference_type>;
  using vert_key_iter = index_iterator<vert_key>;


  // HashGrid(const std::vector<V>& vertices);
  HashGrid(const PointVec<V>& vertices);
  //HashGrid(const int nx, const int ny, const float _sep = 1.0f);
  HashGrid(const glm::vec2& min_pos, const glm::vec2& max_pos, const glm::uvec2& n);

  /*template <typename PointCloud, 
    typename = typename std::enable_if_t<
      !std::is_same_v<PointCloud, HashGrid<V>> &&
      std::is_base_of_v<PointVec<V>, PointCloud>
  >>
  HashGrid<V>& operator=(const PointCloud& other);*/

  void print_info() const;

  bool is_in_range_point(const glm::vec2& p) const;
  bool is_in_range_cell(const glm::uvec2& c) const;
  bool is_empty_cell(const glm::uvec2& c) const;
  bool is_empty_bucket(const uint id) const;

  using PointVec<V>::are_verts_shrinked;
  void shrink_verts();

  using PointVec<V>::empty;
  using PointVec<V>::clear;
  using PointVec<V>::verts_size;

  using PointVec<V>::is_in_range_vert;
  using PointVec<V>::is_removed_vert;

  using PointVec<V>::vert;
  //const V& vert(const vert_key vh) const;
  V& vert(const vert_key vert_id);

  vert_key add_vert(const V& v);
  void remove_vert(const vert_key vert_id);

  using PointVec<V>::begin_verts;
  using PointVec<V>::end_verts;
  using PointVec<V>::verts;


  // Find a vertex by its exact position (within the specified treshhold)
  // If a vertex is not found, it returns an invalid vert_key()
  // The given threshold should be smaller than the grid separation distance
  //vert_key find(const glm::vec2& pos, const float threshhold = 0.01f) const;
  vert_key find(const glm::vec2& pos) const;
  vert_key find(const glm::vec2& pos, const float threshhold) const;

  // Searches all buckets starting from the bucket that the given position is hashed on and then
  //  searches all other buckets starting from nearest buckets to the farthest
  // This always returns a valid vertex handle, if the data structure is not empty
  vert_key find_closest(const glm::vec2& pos) const;
  vert_key find_closest(const vert_key vert_id) const;  // finds the closes vertex to the given vertex


//  template <typename IterType>
//  void add_verts(const IterType begin_verts, const IterType end_verts);

//  void set_grid(const float sep);
//  void set_grid(const glm::vec2& sep, const glm::uvec2& n);
  void set_grid(const glm::vec2& min_pos, const glm::vec2& max_pos, const glm::uvec2& n);


 // ---------------------------------------------------------------------- //

//protected:

public:
  using PointVec<V>::_vertices;

//protected:

public:

  class bucket_iterator;
  class const_bucket_iterator;

  vert_key next_in_bucket(const vert_key vert_id) const;
  vert_key prev_in_bucket(const vert_key vert_id) const;  
  vert_key begin_in_bucket(const vert_key vert_id) const;
  vert_key end_in_bucket() const { return vert_key(); }

  vert_key bucket_to_vert(const uint bucket_id) const;
  uint vert_to_bucket(const vert_key vert_id) const;


  void free_vert(const vert_key vert_id);

  glm::uvec2 pos_to_cell(const glm::vec2& pos) const;
  uint pos_to_index(const glm::vec2& pos) const;

  uint cell_to_index(const glm::uvec2& ipos) const;
  glm::vec2 cell_to_pos(const glm::uvec2& ipos) const;
  glm::uvec2 index_to_cell(const uint index) const;

  void resize_hash(const uint nx, const uint ny, const float _sep);
  void update_hash(const vert_key vert_id);
  void update_hash();

  void add_in_bucket(const vert_key vh);
  void remove_from_bucket(const vert_key vh);

//  void rehash_grid();
//  vert_key closest_vert(const glm::vec2& pos) const;
//  bool bPossibleRehash{};

  //bool cell_x_circle(const glm::uvec2& cell, const glm::vec2& pos, const float radius) const;

  float box_max(const glm::vec2& point, const glm::uvec2& cell) const;
  float box_min(const glm::vec2& point, const glm::uvec2& cell) const;
  
  Iterable<bucket_iterator> bucket(const uint bucket_id);
//  Iterable<typename std::vector<vert_key>::const_iterator> bucket(const uint bucket_id) const;

  // The size of all buckets is n.x * n.y
  glm::uvec2 _n; // number of cells per each dimention

  glm::vec2 _min_pos; // the beginning/origin of the grid
  glm::vec2 _max_pos; // the end of the grid
  glm::vec2 _sep; // the size of a single square cell


  // The buckets that the vertices are stored at
//  std::vector<std::vector<vert_key>> _buckets;

  // A vector of vertex handler, where each index corresponds to a vertex index
  //  and the value gives you the next vertex in the same bucket
  std::vector<vert_key> _next_in_bucket;
  
  // This stores the begin vert_key for each bucket,
  // If the bucket is empty it is set to the end vertex that is vert_key(uint(-1))
  std::vector<vert_key> _buckets;


// ----------------------------------------------------------------- //

/*  class bucket_iterator {
  public:
        
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = vert_key;
    using pointer           = vert_key*;  // or also value_type*
    using reference         = vert_key&;  // or also value_type&

    bucket_iterator(pointer ptr) : _ptr(ptr) {}
    
    reference operator*() { return *_ptr; }
    const value_type& operator*() const { return *_ptr; }
    
    pointer operator->() { return _ptr; } 
    const pointer operator->() const { return _ptr; }

    bucket_iterator& operator++() { _ptr += *_ptr; return *this; }
    bucket_iterator operator++(int) { bucket_iterator temp(*this); ++(*this); return temp; }

    bool operator==(const bucket_iterator& other) const { return _ptr == other._ptr; }
    bool operator!=(const bucket_iterator& other) const { return _ptr != other._ptr; }

  private:
    uint* _ptr;
//    std::vector<vert_key>& _next_in_bucket_ref;
  };
*/


  class bucket_iterator {
  public:
        
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = vert_key;
    using pointer           = vert_key*;  // or also value_type*
    using reference         = vert_key&;  // or also value_type&

//    bucket_iterator(const vert_key vert_id, std::vector<vert_key>& _next_in_bucket_ref)
//      : _vert_id(vert_id), _next_in_bucket_ref(_next_in_bucket_ref) {}

    bucket_iterator(const vert_key vert_id = vert_key(),
                    vert_key* verts_begin_ptr = nullptr
    ) : _vert_id(vert_id), _verts_begin_ptr(verts_begin_ptr) {}

    vert_key operator*() { return _vert_id; }
    const vert_key& operator*() const { return _vert_id; }
    
//    vert_key* operator->() { return _verts_begin_ptr + _vert_id; } 
//    const vert_key* operator->() const { return _verts_begin_ptr + _vert_id; }

    bucket_iterator& operator++() { _vert_id = _verts_begin_ptr[_vert_id]; return *this; }
    bucket_iterator operator++(int) { bucket_iterator temp(*this); ++(*this); return temp; }

    bool operator==(const bucket_iterator& other) const { return _vert_id == other._vert_id; }
    bool operator!=(const bucket_iterator& other) const { return _vert_id != other._vert_id; }

  private:
    vert_key _vert_id;
    //std::vector<vert_key>& _next_in_bucket_ref;
    vert_key* _verts_begin_ptr;
  };


};


  // ================================================================================= //


template <typename V>
inline void HashGrid<V>::print_info() const {
  /*for(const std::vector<vert_key>& bucket : _buckets) {
    uint bucket_id = &bucket - _buckets.data();

    std::cout << bucket_id << ") ";
    
    for(vert_key vh : bucket) std::cout << vh << ", ";

    std::cout << "\n";
  }*/

  for(uint j = 0U; j < _n.y; ++j) {
    for(uint i = 0U; i < _n.x; ++i) {
      const std::vector<vert_key>& bucket = _buckets[cell_to_index(glm::uvec2(i, j))];
      std::cout << bucket.size();
    }
    std::cout << "\n";
  }

}


template <typename V>
inline bool HashGrid<V>::is_in_range_point(const glm::vec2& p) const {
  return p.x >= _min_pos.x && p.y >= _min_pos.y && p.x < _max_pos.x && p.y < _max_pos.y;
}

template <typename V>
inline bool HashGrid<V>::is_in_range_cell(const glm::uvec2& c) const {
//  return c.x >= 0 && c.y >= 0 && c.x < _n.x && c.y < _n.y;
  return c.x < _n.x && c.y < _n.y;
}


template <typename V>
inline bool HashGrid<V>::is_empty_cell(const glm::uvec2& c) const {
  return _buckets[cell_to_index(c)] == vert_key();
}


template <typename V>
inline bool HashGrid<V>::is_empty_bucket(const uint id) const { 
  return _buckets[id] == vert_key();
}


template <typename V>
inline void HashGrid<V>::shrink_verts() {
  for(vert_key vh(this->_removed_verts.size() - 1U); vh != uint(-1); --vh)
    if(is_removed_vert(vh))
      HashGrid<V>::free_vert(vh);

  this->_removed_verts.assign(this->_removed_verts.size(), false);
  this->_removed_verts_count = 0U;
}


template <typename V>
inline HashGrid<V>::HashGrid(const glm::vec2& min_pos, const glm::vec2& max_pos, const glm::uvec2& n)
  : _min_pos(min_pos), _max_pos(max_pos), _n(n),
    _buckets(n.x * n.y, vert_key()),
    _sep((max_pos - min_pos) / glm::vec2(n)) 
    {

//  if (_n.x <= 0 || _n.y <= 0)
//    throw std::invalid_argument("The grid dimentions nx and ny should be greater than zero");
}


template <typename V>
inline V& HashGrid<V>::vert(const vert_key vert_id) {
  if (!is_in_range_vert(vert_id))
    throw std::out_of_range("The handle of the vertex that you want to get is out of range");

//  bPossibleRehash = true;

  //return PointVec<V>::vert(vh);
  return this->_vertices[vert_id];
}


template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::add_vert(const V& v) {
  vert_key vh = PointVec<V>::add_vert(v);
  
  if (this->_vertices.size() != _next_in_bucket.size())
    _next_in_bucket.push_back(vert_key());

  vert_key& buck_begin = _buckets[pos_to_index(v.pos)];
  _next_in_bucket[vh] = buck_begin;
  buck_begin = vh;

  return vh;
}


template <typename V>
inline void HashGrid<V>::remove_vert(const vert_key vert_id) {
  const V& v = _vertices[vert_id];
  
  uint id = pos_to_index(v.pos);
  vert_key& buck_begin = _buckets[id];

  if (buck_begin == vert_id) {
    buck_begin = _next_in_bucket[vert_id];
  }
  else {
    vert_key prev = buck_begin;
    for(; _next_in_bucket[prev] != vert_id; prev = _next_in_bucket[prev]) {}
    _next_in_bucket[prev] = _next_in_bucket[vert_id];
  }

  PointVec<V>::remove_vert(vert_id);
}

// --------------------------------------------------------------- //


/*template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::find(const glm::vec2& pos, const float threshhold) const {

  if (!is_in_range_point(pos))
    throw std::invalid_argument("Out of boundaries point");

  // We will limit the threshold size, so that we only have to check 4 overlapping cells at most
  if (threshhold > _sep.x || threshhold > _sep.y)
    throw std::invalid_argument("The threshold should be smaller than the size of a single cell");

  glm::uvec2 cells[4] = {{uint(-1), uint(-1)}, {uint(-1), uint(-1)}, {uint(-1), uint(-1)}, {uint(-1), uint(-1)}};
  cells[0] = pos_to_cell(pos); 


  // The current/center cell AABB corners
  //glm::vec2 min_pos = _sep * glm::vec2(uint(pos.x / _sep) % _n.x, uint(pos.y / _sep) % _n.y);
  glm::vec2 cell_min_pos = _sep * cells[0];
  glm::vec2 cell_max_pos = cell_min_pos + _sep;

  glm::uvec2 diag;
  glm::uvec2 r;
  glm::uvec2 cell;


  if (pos.x + threshhold > cell_max_pos.x)
    r = glm::uvec2(1, 0);
  else if (pos.x - threshhold < cell_min_pos.x)
    r = glm::uvec2(-1, 0);
  
  cell = cells[0] + r;  
  if (is_in_range_cell(cell)) {
    diag += r;
    cells[1] = cell;
  }


  if (pos.y + threshhold > cell_max_pos.y)
    r = glm::uvec2(0, 1);
  else if (pos.y - threshhold < cell_min_pos.y)
    r = glm::uvec2(0, -1);

  cell = cells[0] + r;
  if (is_in_range_cell(cell)) {
    diag += r;
    cells[2] = cell;
  }

  if (diag.mag2() == 2)
    cells[3] = cells[0] + diag;


  for (const glm::uvec2& c : cells) {
    if (c.x == -1 && c.y == -1)
      continue;
    
    std::vector<vert_key>& bucket = _buckets[cell_to_index(c)];
    for(vert_key vh : bucket) {
      const V& v = _vertices[vh];
      
      if ((pos - v.pos).mag2() < threshhold * threshhold) {
        return vh; 
      }
    }
  }

  return vert_key();
}*/

template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::find(const glm::vec2& pos) const {
  return find(pos, std::min(_sep.x, _sep.y));
}


template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::find(const glm::vec2& pos, const float threshhold) const {

  if (!is_in_range_point(pos))
    throw std::invalid_argument("Out of boundaries point");

  // We will limit the threshold size, so that we only have to check 4 overlapping cells at most
  if (threshhold > _sep.x || threshhold > _sep.y)
    throw std::invalid_argument("The threshold should be smaller than the size of a single cell");

  const glm::uvec2 c = pos_to_cell(pos);  
  const glm::uvec2 cells[9] = {
    {c.x - 1, c.y - 1}, {c.x    , c.y - 1}, {c.x + 1, c.y - 1}, 
    {c.x - 1, c.y    }, {c.x    , c.y    }, {c.x + 1, c.y    },
    {c.x - 1, c.y + 1}, {c.x    , c.y + 1}, {c.x + 1, c.y + 1}
  };

  float min_dist2 = std::numeric_limits<float>::max();
  vert_key min_vert;
  float r2 = threshhold * threshhold;

  for (const glm::uvec2& cell : cells) {
    //if (!is_in_range_cell(cell))
    // continue;
    if (!is_in_range_cell(cell) ||
        is_empty_cell(cell) ||
        box_min(pos, cell) > threshhold
    ) continue;
    
    uint id = cell_to_index(cell);
    const std::vector<vert_key>& bucket = _buckets[id];
    
    for(vert_key vh : bucket) {
      const V& v = _vertices[vh];
      float dist2 = (v.pos - pos).mag2();

      if (dist2 > r2)
        continue;

      if (dist2 < min_dist2) {
        min_vert = vh;
        min_dist2 = dist2;
      }
    }
  }
  
  //return vert_key();
  return min_vert;
}


// NOTE: USE A PRIORITY_QUEUE WITH THE PRIORITY BEING, THE SDF OF A CELL THAT SHOULD BE CHECKED
// Or use some sort of raymarching to partition a group/ring/circle of cells to be checked together
//  one after another in a sequence

// This one has a problem, because of the points that are out of the grid and are overlapped to the other
// side, making them to appear closer when they may actually be the furthest
/*template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::find_closest(const glm::vec2& pos) const {

  using namespace olc;

  if (empty())
    return vert_key();

  static const olc::vi2d relative_cells[8] = {
    {-1, -1}, { 0, -1}, { 1, -1},
    {-1,  0},           { 1,  0},
    {-1,  1}, { 0,  1}, { 1,  1}
  };

  std::vector<vu2d> final_cells;

  std::vector<vu2d> cells;
  cells.push_back(pos_to_cell(pos));

  float radius = 0.0f;
  std::vector<bool> queued;
  std::vector<bool> visited; 
  std::vector<vu2d> new_cells;
  

  while(true) {
    new_cells.clear();

    for(const vu2d& c : cells) {
      radius = std::max(radius, box_max());
      visited[cell_to_index(c)] = true;
    }

    while (!cells.empty()) {
      vu2d cell = cells.back();
      cells.pop_back();

      uint bucket_id = cell_to_index(cell);
//      visited[bucket_id] = true;

//    if (!visited[bucket_id])
//      new_cells.push_back(cell);

      for(const vi2d& rc : relative_cells) {
        vu2d c = cell + rc;
        uint id = cell_to_index(c);

        if (
          !is_in_range_cell(c) && 
          !queued[id] && 
          box_min(pos, c) < radius
        ) {
          cells.push_back(c);
          new_cells.push_back(c);
          queued[id] = true;
        }
      }


    }

  }
*/
  /*
  for (const vu2d& c : cells) {
    if (!is_in_range_cell(c))
      continue;
    if (!is_empty_cell(c))
      final_cells.push_back(c);

  }

  if (!final_cells.empty()) {
    float min_dist = std::numeric_limits<float>::max();
    vert_key min_vert;

    for (const vu2d& c : final_cells) {
      std::vector<vert_key>& bucket = _buckets[cell_to_index(c)];
      
      for(vert_key vh : bucket) {
        const V& v = _vertices[vh];
        
        float d2 = (v.pos - pos).mag2();
        if (d2 < min_dist) {
          min_dist = d2;
          min_vert = vh;
        }
      }
    }

    return min_vert;
  }

  for()
  */

//}


template <typename V>
inline void HashGrid<V>::set_grid(const glm::vec2& min_pos, const glm::vec2& max_pos, const glm::uvec2& n) {
  _n = n;
  _min_pos = min_pos;
  _max_pos = max_pos;

  _sep = (max_pos - min_pos) / n;
}



// ===================================================================== //


/*template <typename V>
inline void HashGrid<V>::rehash_grid() {
  _buckets.clear();
  
  for(vert_key vh : verts()) {
    const V& v = _vertices[vh];
  
    _buckets.
    
  }
}*/


template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::next_in_bucket(const vert_key vert_id) const {
  return _next_in_bucket(vert_id);
}

template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::prev_in_bucket(const vert_key vert_id) const {
  vert_key prev_id = begin_in_bucket(vert_id);
  
  for(; 
    next_in_bucket(prev_id) != vert_id;
    prev_id = next_in_bucket(prev_id))
  {}
  
  return prev_id;
}

template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::begin_in_bucket(const vert_key vert_id) const {
  return bucket_to_vert(vert_to_bucket(vert_id));
}


template <typename V>
inline typename HashGrid<V>::vert_key HashGrid<V>::bucket_to_vert(const uint bucket_id) const {
  return _buckets[bucket_id];
}

template <typename V>
inline uint HashGrid<V>::vert_to_bucket(const vert_key vert_id) const {
  const V& v = _vertices[vert_id];
  return pos_to_index(v.pos);
}


template <typename V>
inline void HashGrid<V>::free_vert(const vert_key vert_id) {
  assert(is_in_range_vert(vert_id) && is_removed_vert(vert_id));

  if (vert_id + 1U != verts_size()) {
    this->_vertices[vert_id] = std::move(this->_vertices.back());
    this->_next_in_bucket[vert_id] = this->_next_in_bucket.back();
    //_removed_verts[vert_id] = _removed_verts.back();
  }


  // We need to also update the previous _next_in_bucket
  //  that pointed to the last vertex that we moved at
  //  the place of the removed one

  vert_key last_id(uint(_vertices.size()) - 1);
  const V& v = _vertices[last_id];
  vert_key& buck_begin = _buckets[pos_to_index(v.pos)];

  if (buck_begin == last_id) {
    buck_begin = vert_id;
  }
  else {
    vert_key prev = buck_begin;
    for(; _next_in_bucket[prev] != last_id; prev = _next_in_bucket[prev]) {}
    _next_in_bucket[prev] = vert_id;
  }


  this->_vertices.pop_back();
  this->_removed_verts.pop_back();
  this->_next_in_bucket.pop_back();

  //_removed_verts[vert_id] = false;
  //--_removed_verts_count;
}


template <typename V>
inline glm::uvec2 HashGrid<V>::pos_to_cell(const glm::vec2& pos) const {
  assert(is_in_range_point(pos) && "Out of boundary point");
  return (pos - _min_pos) / _sep;
}

template <typename V>
inline uint HashGrid<V>::pos_to_index(const glm::vec2& pos) const {
  return cell_to_index(pos_to_cell(pos));
}

// 2D grid integer position to 1D bucket id
template <typename V>
inline uint HashGrid<V>::cell_to_index(const glm::uvec2& ipos) const {
  assert(is_in_range_cell(ipos) && "The given cell position is out of range");
  return ipos.y * _n.x + ipos.x;
}


template <typename V>
inline glm::vec2 HashGrid<V>::cell_to_pos(const glm::uvec2& ipos) const {
  return glm::vec2(ipos) * _sep + _min_pos;
}

// 1D bucket id to 2D grid integer position
// But this gives a position only in the range from (0, 0) to (nx * _sep, ny * _sep)
template <typename V>
inline glm::uvec2 HashGrid<V>::index_to_cell(const uint index) const {
  assert(index < _buckets.size() && "Bucket index out of range");
  return glm::uvec2(index % _n.x, index / _n.x);
  //return glm::uvec2(index % _n.x, (index / _n.x) % _n.y);
}


template <typename V>
inline void HashGrid<V>::resize_hash(const uint nx, const uint ny, const float _sep) {
  
}


template <typename V>
inline void HashGrid<V>::update_hash(const vert_key vh) {
  
}


template <typename V>
inline void HashGrid<V>::update_hash() {
  for(vert_key vh : verts())
    if (!is_removed_vert(vh))
      update_hash(vh);
}

// Maximum distance from a point to the surface of a box cell
template <typename V>
inline float HashGrid<V>::box_max(const glm::vec2& point, const glm::uvec2& cell) const {
  glm::vec2 cell_center = cell_to_pos(cell) + 0.5f * _sep;
  return sdf::box_max(point - cell_center, 0.5f * _sep);
}


// Minimum signed distance from a point to the surface of a box cell
template <typename V>
inline float HashGrid<V>::box_min(const glm::vec2& point, const glm::uvec2& cell) const {
  glm::vec2 cell_center = cell_to_pos(cell) + 0.5f * _sep;
  return sdf::box(point - cell_center, 0.5f * _sep);
}


template <typename V>
inline Iterable<typename HashGrid<V>::bucket_iterator>
    HashGrid<V>::bucket(const uint bucket_id) {

  //return Iterable<typename std::vector<vert_key>::iterator>();

  //return { _buckets[bucket_id], bucket_id };
  return { 
    bucket_iterator(_buckets[bucket_id], _next_in_bucket.data()),
    bucket_iterator(vert_key(-1), _next_in_bucket.data())
  };
}




} // namespace pc

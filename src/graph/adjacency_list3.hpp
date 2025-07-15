#pragma once

#include "../try2/handle/handles.hpp"
#include "../iterators/iterable.hpp"
#include "../iterators/index_iterator.hpp"
#include "../try3/jagged_mat.hpp"
// #include "../try2/handle/key_vector.hpp"
#include "../iterators/flat_iterator.hpp"
#include <type_traits>

#include <array>
#include <vector>
#include <set>
// #include <utility> // std::pair
#include <iterator>  // reverse_iterator

// NOTE:
// How do you:
// 1) Combine it with different PointClouds
// 2) Insert Garbage Collection system for vertices and edges
// 3) Add properties

// Directed Graph Data Structure
// template <typename VertKey = index_vert_handle<std::ptrdiff_t>>
class AdjacencyList {
public:

  // using vert_key = VertKey; // vertex descriptor/identifier/handle
  using vert_key = index_vert_handle<std::ptrdiff_t>;
  using edge_key = index_edge_handle<std::ptrdiff_t>;
  
  using vert_key_iter = index_iterator<vert_key>;
  using edge_key_iter = index_iterator<edge_key>;
  // using vert_key_rev_iter = std::reverse_iterator<vert_key_iter>;
  // using edge_key_rev_iter = std::reverse_iterator<edge_key_iter>;

  using container = JaggedMatrix<vert_key>;
  using difference_type = typename container::difference_type;
  using size_type = typename container::size_type;

  // NOTE: You could represent every unique edge with
  //  only a single index for the flat array of vert_keys
  //  but you would have verty hard time finding the source vertex
  // using edge_key = index_edge_handle<std::ptrdiff_t>;

  // NOTE:
  // 0 is from/source vertex, 1 is to/target vertex
  // For this you would need a lookup_iterator and index_iterator mix that will result in bidirectional iterator
  // using edge_key_ext = std::pair<vert_key, edge_key>; 


  // NOTE:
  // We let the user to use those containers as properties
  //  so when he adds or removes them, they are syncronized
  //  with the resize logic of the vertices and edges
  //  that are stored here
  //
  // template <typename T>
  // using vert_prop_type = KeyVector<std::vector<T>, vert_key>;
  //
  // template <typename T>
  // using edge_prop_type = KeyVector<std::vector<T>, edge_key>;

protected:

  void check_vert_id(vert_key vi) const {
    if (!is_valid_vert(vi))
      throw std::out_of_range("Invalid vertex index");
  }

  void check_edge_id(edge_key e) const {
    if (!is_valid_edge(e))
      throw std::out_of_range("Invalid edge index");
  }

  vert_key& __head(edge_key e) {
    return _adj.flat()[e];
  }

private:

  // static_assert(std::is_convertible_v<vert_key, std::ptrdiff_t>);
  // static_assert(std::is_constructible_v<vert_key, std::ptrdiff_t>);
  // static_assert(std::is_convertible_v<edge_key, std::ptrdiff_t>);
  // static_assert(std::is_constructible_v<edge_key, std::ptrdiff_t>);

  // JaggedMatrix<vert_key> _edges;
  container _adj; // stores every adjacent vertex key, per each vertex

public:

  AdjacencyList()
  : _adj() {}

  AdjacencyList(
    size_type size_of_verts,
    const std::vector<std::array<vert_key, 2>>& edges = {}
  ) : _adj(size_of_verts) {
    // reserve_verts(size_of_verts);
    // for(size_type i = 0; i < size_of_verts; ++i)
    //   add_vert();
    reserve_edges(edges.size());
    for(const auto& e : edges)
      add_edge(e[0], e[1]);
  }

  // NOTE:
  // If Upper class has a user-declared copy constructor, 
  //  copy assignment, or destructor, the compiler does
  //  not generate move operations !


  bool is_valid_vert(vert_key key) const {
    return 0 <= key && key < verts_size();
  }

  bool is_valid_edge(edge_key key) const {
    return 0 <= key && key < edges_size();
  }

  void reserve_verts(size_type num) {
    _adj.reserve(num);
  }

  void reserve_edges(size_type num) {
    _adj.reserve_flat(num);
  }
  
  size_type verts_size() const {
    return _adj.size();
  }

  size_type edges_size() const {
    return _adj.flat().size();
  }

  vert_key_iter verts_begin() const {
    return vert_key_iter(vert_key(0));
  }

  vert_key_iter verts_end() const {
    return vert_key(verts_size());
  }
  
  edge_key_iter edges_begin() const {
    return edge_key(0);
  }
  
  edge_key_iter edges_end() const {
    return edge_key(edges_size());
  }

  Iterable<vert_key_iter> verts() const {
    return {verts_begin(), verts_end()};
  }

  Iterable<edge_key_iter> edges() const {
    return {edges_begin(), edges_end()};
  }
  
  // Adjacent Vertices to the given vert_id
  container::const_iterator verts(vert_key vert_id) const {
    check_vert_id(vert_id);
    return _adj[vert_id];
  }

  // NOTE:
  // Outgoing Edges of the vertex (so the given vert_id is their source vertex)
  Iterable<edge_key_iter> edges(vert_key vert_id) const {
    check_vert_id(vert_id);
    auto range = _adj[vert_id];
    return {
      edge_key_iter(edge_key(range.begin() - _adj.flat().begin())),
      edge_key_iter(edge_key(range.end() - _adj.flat().begin()))
    };
  }


  bool empty() const {
    return _adj.empty();
  }
  
  // (head | target | to | front) vertex of an edge
  vert_key head(edge_key e) const {
    check_edge_id(e);
    return _adj.flat()[e];
  }

  // (tail | source | from | back) vertex of an edge
  vert_key tail(edge_key e) const {
    check_edge_id(e);
   
    for(vert_key vi : verts())
      if (e < _adj[vi].end() - _adj.flat().begin())
        return vi;

    return vert_key(-1);
  }

  edge_key edge(vert_key tail_id, vert_key head_id) const {
    check_vert_id(tail_id);

    for(edge_key e : edges(tail_id))
      if (head(e) == head_id)
        return e;

    return edge_key(-1);
  }

  // ---------------------------------------------------


  void clear() {
    _adj.clear();
  }


  // vert_key on_add_vert() {
  vert_key add_vert() {
    vert_key key(_adj.size());
    _adj.insert(_adj.end(), {});
    return key;
  }

  // void on_remove_vert(vert_key key) {
  // TODO: This will be very expensive rearrangment of the whoe _adj flat array
  void remove_vert(vert_key vert_id) {
    check_vert_id(vert_id);
    _adj.erase(_adj.begin() + vert_id);

    auto row_it = _adj.begin();
    auto v_it = _adj.flat().begin();

    // NOTE: row_it could be invalidated after flat erase,
    // because internaly its stores the same type flat iterator
    // but i think this doesnt happen with most stl containers
    while(v_it != _adj.flat().end()) {
      row_it += v_it >= row_it->end();
      // if (v_it >= row_it->end())
      //   ++row_it;
      
      vert_key& vi(*v_it);

      if (vi == vert_id) {
        v_it = _adj.erase(row_it, v_it);
        continue;
      }

      vi -= vi > vert_id;
      ++v_it;
    }
  }


  // void remove_vert(vert_key vert_id) {
  //   check_vert_id(vert_id);
  //   _adj.erase(_adj.begin() + vert_id);
  //
  //   for(vert_key source : reversed(verts()))
  //     for(edge_key e : reversed(edges(source))) {
  //       vert_key& target = _adj.flat()[e]; // head ref
  //       
  //       if (target == vert_id) {
  //         remove_edge(e, source);
  //         continue;
  //       }
  //
  //       target -= target > vert_id;
  //     }
  // }


  edge_key add_edge(vert_key source_id, vert_key target_id) {
    check_vert_id(source_id);
    check_vert_id(target_id);
    auto source_it(_adj.begin() + source_id);
    return edge_key(
      _adj.insert(
        source_it,
        source_it->end(),
        target_id
      ) - _adj.flat().begin()
    );
  }


  // Because to get the tail vertex is O(V) complexity
  //  we give the option to also pass a tail vertex
  //  explicitly, to speed up the removal
  void remove_edge(edge_key e, vert_key tail) {
    check_edge_id(e);
    check_vert_id(tail);
    auto vert_it = _adj.begin() + tail;
    auto edge_it = _adj.flat().begin() + e;
    _adj.erase(vert_it, edge_it);
  }
  
  void remove_edge(vert_key tail, vert_key head) {
    remove_edge(edge(tail, head), tail);
  }

  void remove_edge(edge_key e) {
    remove_edge(e, tail(e));
  }


};



// class AdjacencyListG
// : public AdjacencyList
// {
// public:
//
//   using BaseType = AdjacencyList;
//   
//   using BaseType::vert_key;
//   using BaseType::edge_key;
//   using BaseType::size_type;
//
// private:
//
//   std::set<vert_key> _removed_verts;
//   std::set<edge_key> _removed_edges;
//
// public:
//
//   using BaseType::BaseType;
//
//   bool are_verts_shrinked() const {
//     return _removed_verts.empty();
//   }
//
//   bool are_edges_shrinked() const {
//     return _removed_edges.empty();
//   }
//
//   size_type removed_verts() const {
//     return _removed_verts.size();
//   }
//
//   size_type removed_edges() const {
//     return _removed_edges.size();
//   }
//
//   bool is_vert_removed(vert_key vert_id) const {
//     return _removed_verts.contains(vert_id);
//   }
//
//   bool is_edge_removed(edge_key e) const {
//     return _removed_edges.contains(e);
//   }
//
//   edge_key add_edge(vert_key source, vert_key target) {
//     if (are_edges_shrinked())
//       return BaseType::add_edge(source, target);
//
//     auto range = edges(source);
//     auto it = _removed_edges.lower_bound(*range.begin());
//     edge_key e = *it;
//
//     if (e < *range.end()) {
//       _removed_edges.erase(it);
//       __head(e) = target;
//       return e;
//     }
//     else
//       return BaseType::add_edge(source, target);
//   }
//
//   void remove_edge(edge_key e) {
//     _removed_edges.insert(e);
//   }
//
//
//   vert_key add_vert() {
//     auto it = _removed_verts.begin();
//     if (it == _removed_verts.end())
//       return BaseType::add_vert();
//     
//     vert_key key(*it);
//     _removed_verts.erase(it);
//     return key;
//   }
//
//   void remove_vert(vert_key vert_id) {
//     _removed_verts.insert(vert_id);
//     for(edge_key e : edges())
//       if (head(e) == vert_id)
//         remove_edge(e);
//   }
//
//
//   void shrink_verts() {
//     Iterable range(_removed_verts.begin(), _removed_verts.end());
//     for(vert_key vi : reversed(range))
//       BaseType::remove_vert(vi);
//     _removed_verts.clear();
//   }
//   
//   void shrink_edges() {
//     Iterable range(_removed_edges.begin(), _removed_edges.end());
//     for(edge_key e : reversed(range))
//       BaseType::remove_edge(e);
//     _removed_edges.clear();
//   }
//
//   void shrink() {
//     Iterable vrange(_removed_verts.begin(), _removed_verts.end());
//     // Iterable erange(_removed_edges.begin(), _removed_edges.end());
//    
//     for(vert_key source : reversed(verts()))
//       for(edge_key e : reversed(edges(source)))
//         if (is_edge_removed(e) || is_vert_removed(head(e)))
//           BaseType::remove_edge(e, source);
//     
//     for (vert_key vi : reversed(vrange))
//       BaseType::remove_vert(vi);
//
//     _removed_edges.clear();
//     _removed_verts.clear();
//   }
//
//
// };


class AdjacencyListG
: public AdjacencyList
{
public:

  using BaseType = AdjacencyList;
  
  using BaseType::vert_key;
  using BaseType::edge_key;
  using BaseType::size_type;

private:

  // std::vector<bool> _removed_edges;
  std::vector<int> _removed_edges_per_vert; // stores removed on back
  // JaggedMatrix<bool> _removed_edges;
  std::vector<bool> _removed_verts_mask;
  size_type _removed_verts_count;

  using BaseType::edges;

public:

  Iterable<edge_key_iter> edges(vert_key source) const {
    auto range = BaseType::edges(source);
    return {
      range.begin(),
      range.end() - removed_edges(source);
    };
  }

// ---------------------------------------------------------

  using BaseType::BaseType;

  bool are_verts_shrinked() const {
    return !_removed_verts_count;
  }

  bool are_edges_shrinked(vert_key vi) const {
    return !_removed_edges_per_vert[vi];
  }

  size_type removed_verts() const {
    return _removed_verts_count;
  }

  size_type removed_edges(vert_key vi) const {
    return _removed_edges_per_vert[vi];
  }

  bool is_vert_removed(vert_key vert_id) const {
    return _removed_verts_mask[vert_id];
  }

  // bool is_edge_removed(edge_key e) const {
  //   return _removed_edges.contains(e);
  // }

  edge_key add_edge(vert_key source, vert_key target) {
    if (are_edges_shrinked(source))
      return BaseType::add_edge(source, target);

    edge_key e = (edges(source).end() - _removed_edges_per_vert[source]);
    BaseType::_adj.flat()[e] = target;
    return e;
  }

  // Pop and Swap per vertex adjacent edges
  void remove_edge(edge_key e, vert_key source) {
    vert_key last = *(verts(source).end() - 1 - _removed_edges_per_vert[source]);
    // vert_key last = BaseType::_adj(source + 1, -1 - _removed_edges_per_vert[source]);
    BaseType::_adj.flat()[e] = last; 
    ++_removed_edges_per_vert[source];
  }

  void remove_edge(edge_key e) {
    remove_edge(e, tail(e));
  }

  void remove_edge(vert_key source, vert_key target) {
    remove_edge(edge(source, target), source);
  }


  vert_key add_vert() {
    auto it = _removed_verts.begin();
    if (it == _removed_verts.end())
      return BaseType::add_vert();
    
    vert_key key(*it);
    _removed_verts.erase(it);
    return key;
  }

  void remove_vert(vert_key vert_id) {
    _removed_verts.insert(vert_id);
    for(edge_key e : edges())
      if (head(e) == vert_id)
        remove_edge(e);
  }


  void shrink_verts() {
    Iterable range(_removed_verts.begin(), _removed_verts.end());
    for(vert_key vi : reversed(range))
      BaseType::remove_vert(vi);
    _removed_verts.clear();
  }
  
  void shrink_edges() {
    Iterable range(_removed_edges.begin(), _removed_edges.end());
    for(edge_key e : reversed(range))
      BaseType::remove_edge(e);
    _removed_edges.clear();
  }

  void shrink() {
    Iterable vrange(_removed_verts.begin(), _removed_verts.end());
    // Iterable erange(_removed_edges.begin(), _removed_edges.end());
   
    for(vert_key source : reversed(verts()))
      for(edge_key e : reversed(edges(source)))
        if (is_edge_removed(e) || is_vert_removed(head(e)))
          BaseType::remove_edge(e, source);
    
    for (vert_key vi : reversed(vrange))
      BaseType::remove_vert(vi);

    _removed_edges.clear();
    _removed_verts.clear();
  }


};



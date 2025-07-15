#pragma once

#include "../keys/keys.hpp"
// #include "../key_cont/adaptors/key_matrix.hpp"
#include "../iterators/iterable.hpp"
#include "../iterators/index_iterator.hpp"
#include "../matrices/jagged_matrix.hpp"
// #include "../try2/handle/key_vector.hpp"
// #include "../iterators/flat_iterator.hpp"
#include "../keys/garbage_policy.hpp"

#include <array>
#include <vector>
#include <type_traits>
// #include <utility> // std::pair


// Directed Graph Data Structure
// template <typename VertKey = index_vert_handle<std::ptrdiff_t>>
class BaseAdjacencyList 
// : public IndexGarbagePolicy<AdjacencyList, index_vert_handle<std::ptrdiff_t>>
// , public IndexGarbagePolicy<AdjacencyList, index_edge_handle<std::ptrdiff_t>>
{
public:

  // using vert_key = VertKey; // vertex descriptor/identifier/handle
  using vert_key = index_vert_handle<std::ptrdiff_t>;
  using edge_key = index_edge_handle<std::ptrdiff_t>;
  
  // using vert_key_iter = index_iterator<vert_key>;
  // using edge_key_iter = index_iterator<edge_key>;
  using vert_key_iter = index_handle_iterator<vert_key>;
  using edge_key_iter = index_handle_iterator<edge_key>;

  // using container = JaggedMatrix<std::vector<vert_key>>;
  using container = KeyMatrix<vert_key, edge_key, vert_key, JaggedMatrix>;
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

  void check_id(vert_key vi) const {
    if (!contains(vi))
      throw std::out_of_range("Invalid vertex index");
  }

  void check_id(edge_key e) const {
    if (!contains(e))
      throw std::out_of_range("Invalid edge index");
  }

private:

  // static_assert(std::is_convertible_v<vert_key, std::ptrdiff_t>);
  // static_assert(std::is_constructible_v<vert_key, std::ptrdiff_t>);
  // static_assert(std::is_convertible_v<edge_key, std::ptrdiff_t>);
  // static_assert(std::is_constructible_v<edge_key, std::ptrdiff_t>);

  // JaggedMatrix<vert_key> _edges;
  container _adj; // stores every adjacent vertex key, per each vertex

public:

  BaseAdjacencyList()
  : _adj() {}

  BaseAdjacencyList(
    size_type size_of_verts,
    const std::vector<std::array<vert_key, 2>>& edges = {}
  ) : _adj(size_of_verts) {
    // reserve_verts(size_of_verts);
    // for(size_type i = 0; i < size_of_verts; ++i)
    //   add_vert();
    reserve<edge_key>(edges.size());
    for(const auto& e : edges)
      add(e[0], e[1]);
  }

  // NOTE:
  // If Upper class has a user-declared copy constructor, 
  //  copy assignment, or destructor, the compiler does
  //  not generate move operations !


  bool contains(vert_key key) const {
    return vert_key(0) <= key && key < vert_key(size<vert_key>());
  }

  bool contains(edge_key key) const {
    return edge_key(0) <= key && key < edge_key(size<edge_key>());
  }

  template <typename KeyType>
  void reserve(size_type);

  template <>
  void reserve<vert_key>(size_type num) {
    _adj.reserve(num);
  }

  template <>
  void reserve<edge_key>(size_type num) {
    _adj.reserve_flat(num);
  }
  
  template <typename>
  size_type size() const;

  template <>
  size_type size<vert_key>() const {
    return _adj.size();
  }
  
  template <>
  size_type size<edge_key>() const {
    return _adj.flat().size();
  }

  template <typename KeyType>
  index_handle_iterator<KeyType> begin() const;
  
  template <typename KeyType>
  index_handle_iterator<KeyType> end() const;

  template <>
  vert_key_iter begin<vert_key>() const {
    return vert_key_iter(vert_key(0));
  }

  template <>
  vert_key_iter end<vert_key>() const {
    return vert_key(size<vert_key>());
  }
  
  template <>
  edge_key_iter begin<edge_key>() const {
    return edge_key(0);
  }
  
  template <>
  edge_key_iter end<edge_key>() const {
    return edge_key(size<edge_key>());
  }

  Iterable<vert_key_iter> verts() const {
    return {begin<vert_key>(), end<vert_key>()};
  }

  Iterable<edge_key_iter> edges() const {
    return {begin<edge_key>(), end<edge_key>()};
  }

  template <typename KeyType>
  Iterable<index_handle_iterator<KeyType>> range() const;

  template <>
  Iterable<vert_key_iter> range<vert_key>() const {
    return {begin<vert_key>(), end<vert_key>()};
  }

  template <>
  Iterable<edge_key_iter> range<edge_key>() const {
    return {begin<edge_key>(), end<edge_key>()};
  }


  // Adjacent Vertices to the given vert_id
  typename container::const_reference verts(vert_key vert_id) const {
    check_id(vert_id);
    return _adj[vert_id];
  }

  // NOTE:
  // Outgoing Edges of the vertex (so the given vert_id is their source vertex)
  Iterable<edge_key_iter> edges(vert_key vert_id) const {
    check_id(vert_id);
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
    check_id(e);
    return _adj.flat()[e];
  }

  // (tail | source | from | back) vertex of an edge
  vert_key tail(edge_key e) const {
    check_id(e);
   
    for(vert_key vi : verts())
      if (e < edge_key(_adj[vi].end() - _adj.flat().begin()))
        return vi;

    return vert_key(-1);
  }

  edge_key edge(vert_key tail_id, vert_key head_id) const {
    check_id(tail_id);

    for(edge_key e : edges(tail_id))
      if (head(e) == head_id)
        return e;

    return edge_key(-1);
  }

  // ---------------------------------------------------


  void clear() {
    _adj.clear();
  }

  template <typename KeyType>
  KeyType add();

  // vert_key on_add_vert() {
  template <>
  vert_key add<vert_key>() {
    vert_key key(_adj.size());
    _adj.insert(_adj.end(), {});
    return key;
  }

  // void on_remove_vert(vert_key key) {
  // TODO: This will be very expensive rearrangment of the whoe _adj flat array
  void remove(vert_key vert_id) {
    check_id(vert_id);
    _adj.erase(_adj.begin() + difference_type(vert_id));

    vert_key_iter row_it = _adj.begin();
    edge_key_iter v_it = _adj.flat().begin();

    // NOTE: row_it could be invalidated after flat erase,
    // because internaly its stores the same type flat iterator
    // but i think this doesnt happen with most stl containers
    while(v_it != _adj.flat().end()) {
      row_it += v_it >= _adj[*row_it].end();
      // if (v_it >= row_it->end())
      //   ++row_it;
      
      vert_key& vi = _adj.at(*v_it);

      if (vi == vert_id) {
        v_it = _adj.erase(row_it, v_it);
        continue;
      }

      vi -= vi > vert_id;
      ++v_it;
    }
  }


  edge_key add(vert_key source_id, vert_key target_id) {
    check_id(source_id);
    check_id(target_id);
    auto source_it(_adj.begin() + difference_type(source_id));
    return edge_key(
      _adj.insert(
        source_it,
        // source_it->end(),
        _adj.at(*source_it).end(),
        target_id
      ) - _adj.flat().begin()
    );
  }


  // Because to get the tail vertex is O(V) complexity
  //  we give the option to also pass a tail vertex
  //  explicitly, to speed up the removal
  void remove(edge_key e, vert_key tail) {
    check_id(e);
    check_id(tail);
    auto vert_it = _adj.begin() + difference_type(tail);
    auto edge_it = _adj.flat().begin() + difference_type(e);
    _adj.erase(vert_it, edge_it);
  }
  
  void remove(vert_key tail, vert_key head) {
    remove(edge(tail, head), tail);
  }

  void remove(edge_key e) {
    remove(e, tail(e));
  }


};


template <typename V, typename E>
struct AdjacencyList 
: public BaseAdjacencyList
, public MaskGarbagePolicy<AdjacencyList<V, E>, BaseAdjacencyList::vert_key>
, public MaskGarbagePolicy<AdjacencyList<V, E>, BaseAdjacencyList::edge_key> {

  using BaseAdjacencyList::BaseAdjacencyList;

};







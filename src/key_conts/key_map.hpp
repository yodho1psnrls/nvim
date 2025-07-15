#pragma once

#include "../iterators/transform_iterator.hpp"
#include "../utilities.hpp" // PairToFirst functor class

#include <stdexcept>
#include <iterator>
#include <type_traits>

// To generalize the map similar to vector
//  (in terms of adding/inserting new elements)
//  we need some policy for generating new keys
//  automatically, instead of specifiyng them
//  because to generalize them we need the
//  container to generate keys for use when we
//  insert/insert new elements

// Find out how the range based insert/erase works
// for set, map, unordered_set and unordered_map


// This should work both with std::map and std::unordered_map
template <typename MapType>
class KeyMap {

  MapType _map;

  // Helper function for the key-only iterator
  //  that we need here
  // static typename MapType::key_type get_key(
  //   typename MapType::const_reference kvp
  // ) {
  //   return kvp.first;
  // }

public:

  //using container = std::map<Key, T, Prev, Alloc>;
  // using container = MapType;
  using base_type = MapType;

  using key_type = typename base_type::key_type;
  using value_type = typename base_type::mapped_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using allocator_type = typename base_type::allocator_type;

  // Constructors
  KeyMap(const base_type& other) : MapType(other) {}

  template <typename ...Args>
  KeyMap(Args&&... args)
  : _map(std::forward<Args>(args)...) {}
  
  KeyMap(const std::initializer_list<typename MapType::value_type>& list)
  : _map(list) {}

  bool empty() const { return _map.empty(); }
  size_type size() const { return _map.size(); }
  void clear() { _map.clear(); }

  const base_type& base() const { return _map; }

private:

    // using func_type = std::pair<key_type, value_type>(key_type);

public:
 
  // using key_iterator = key_iterator<typename container::const_iterator>;
  // using key_iterator = transform_iterator<typename MapType::iterator, decltype(get_key)>;
  // using key_iterator = transform_iterator<typename MapType::const_iterator, func_type>;
  using key_iterator = TransformIterator<typename MapType::const_iterator, PairToFirst<typename MapType::value_type>>;
  // using key_iterator = transform_iterator<typename MapType::const_reverse_iterator, PairToFirst<typename MapType::value_type>>;

  using iterator = key_iterator;
  using const_iterator = key_iterator;
  // using reverse_iterator = key_reverse_iterator;
  // using const_reverse_iterator = key_reverse_iterator;


  // NOTE:
  // You would need to rewrite all operators, because
  //  they are inherited and they return a reference
  //  to MapType::iterator instead
  // struct key_type : public MapType::iterator {
  // }


  // using const_iterator = key_iterator;
  // using iterator = key_iterator;
  // using const_reverse_iterator = iterator<typename container::const_reverse_iterator>;
  // using reverse_iterator = const_reverse_iterator;
 
  key_iterator begin() const {
    return key_iterator(_map.cbegin());
  }

  key_iterator end() const {
    return key_iterator(_map.cend());
  }


  key_iterator find(key_type key) const {
    return key_iterator(_map.find(key));
  }
  
  bool contains(key_type key) const {
    // return _map.find(key) != _map.end();
    return _map.contains(key);
  }


  // template <typename IT = key_iterator>
  // typename std::enable_if<
  //   std::is_base_of_v<
  //     std::bidirectional_iterator_tag,
  //     typename std::iterator_traits<IT>::iterator_category>, 
  //   std::reverse_iterator<IT>&>::type
  // rbegin() const { return end(); }
  //
  // template <typename IT = key_iterator>
  // typename std::enable_if<
  //   std::is_base_of_v<
  //     std::bidirectional_iterator_tag,
  //     typename std::iterator_traits<IT>::iterator_category>, 
  //   std::reverse_iterator<IT>&>::type
  // rend() const { return begin(); }


  // NOTE:
  // Here, instead of inserting a range of {key, value} pairs,
  //  we separate the logic, by providing a separate
  //  KeyIter and ValueIter
  // Note that at insert, the KeyIter here should not be the key_iterator
  //  of this class, because it will just overwrite all the elements

  
  template <typename KeyIter>
  key_iterator insert(KeyIter key_it, const value_type& val = {}) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(std::is_convertible_v<typename std::iterator_traits<KeyIter>::value_type, key_type>);
    key_iterator it = _map.insert({*key_it, val});
    return it;
  }

  template <typename KeyIter>
  key_iterator erase(KeyIter key_it) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(std::is_convertible_v<typename std::iterator_traits<KeyIter>::value_type, key_type>);
    extern key_iterator it;
    if constexpr (std::is_same_v<KeyIter, key_iterator>)
      key_iterator it = _map.erase(key_it);
    else
      key_iterator it = _map.erase(*key_it);
    return it;
  }

 
  reference operator[](key_type key) {
    return _map.at(key);
  }

  const_reference operator[](key_type key) const {
    return _map.at(key);
  }

  reference at(key_type key) {
    return _map.at(key);
  }

  const_reference at(key_type key) const {
    return _map.at(key);
  }

};


struct NoKeyIter {};


template <typename MapType, typename KeyIter>
// template <typename MapType, typename KeyIter = NoKeyIter>
class GenKeyMap {

  MapType _map;

  KeyIter _begin;
  KeyIter _end;

  // make the base type add and remove private
  // using BaseType::add;
  // using BaseType::remove;

public:
  
  // using container = MapType;
  using base_type = MapType;

  using key_type = typename base_type::key_type;
  using value_type = typename base_type::mapped_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;

  using key_iterator = KeyIter;
  using reverse_key_iterator = std::reverse_iterator<KeyIter>;
  
  static_assert(std::is_convertible_v<
    typename std::iterator_traits<KeyIter>::value_type,
    key_type
  >);

  GenKeyMap(const KeyIter& begin, const base_type& map)
  : _begin(begin), _end(begin), base_type(map) {}

  template <typename ...Args>
  GenKeyMap(const KeyIter& begin, Args&&... args)
  : _begin(begin), _end(begin), base_type(std::forward<Args>(args)...) {}
  

  void clear() {
    _end = _begin;
    _map.clear();
  }

  bool empty() const {
    return _begin == _end;
  }

  size_type size() const {
    return _map.size();
    // return std::distance(_begin, _end);
  }

  const base_type& base() const {
    return _map;
  }

  bool contains(key_type key) const {
    return _map.contains(key);
  }

  key_iterator find(key_type key) const {
    return key_iterator(key);
  }


  key_iterator begin() const { return _begin; }
  key_iterator end() const { return _end; }
  reverse_key_iterator rbegin() const { return _end; }
  reverse_key_iterator rend() const { return _begin; }

  
  key_iterator add() {
    auto temp = _end;
    ++_end;
    _map.insert({*temp, value_type()});
    return temp;
  }

  key_iterator remove() {
    --_end;
    _map.erase(*_end);
    return _end;
  }
  

  reference operator[](key_type key) {
    return _map.at(key);
  }

  const_reference operator[](key_type key) const {
    return _map.at(key);
  }

  reference at(key_type key) {
    return _map.at(key);
  }

  const_reference at(key_type key) const {
    return _map.at(key);
  }

};



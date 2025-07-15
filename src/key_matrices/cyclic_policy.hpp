#pragma once
#include "../cont_adaptors/traits.hpp" // is_key_container_v
#include "../iterators/tensor_iterator.hpp"
#include "../iterators/jump_iterator.hpp"
#include "../iterators/index_iterator.hpp"
#include "../key_kernel/keys.hpp"

#include <vector>
#include <utility>

template <typename IndexIter, int Index = 0>
class CycleByIndexValue {
// template <int Index, typename IndexIter>
// class __JumpByIndexValue {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using policy_category = nonfixed_jump_policy_tag;

  template <int ID>
  using indexed = CycleByIndexValue<IndexIter, ID>;

  // JumpByIndexValue() = default;
  CycleByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}
  // __JumpByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}
  
  template <typename IT>
  CycleByIndexValue(const CycleByIndexValue<IT, Index>& other) : _id_it(other.base()) {}

  template <int ID>
  operator JumpByIndexValue<IndexIter, ID>() const { return _id_it; }

  const IndexIter& base() const { return _id_it; }
  difference_type value() const { return *_id_it; }
  // const JumpByIndexValue& policy() const { return *this; }
  
  //template <typename IT>
  //JumpByIndexValue(const jump_iterator<IT, JumpByIndexValue>&) = delete;

// protected:
public:
 
  void advance_alone() { _id_it += *_id_it; }

  template <typename IT>
  // template <typename IT, typename = std::enable_if_t<is_iterator_v<IT>>>
  void advance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += *_id_it;
    _id_it += *_id_it;
  }

};

// template <typename IterType>
// CycleByIndexValue(IterType)
// -> CycleByIndexValue<IterType>;


// TODO:
/*template <typename IndexIter, int Index = 0>
class CycleByIndexDiff {
// template <int Index, typename IndexIter>
// class __JumpByIndexValue {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;
  // index_iterator
  // lookup_iterator
  // NOTE: You need to track the current index

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using policy_category = nonfixed_jump_policy_tag;

  template <int ID>
  using indexed = CycleByIndexValue<IndexIter, ID>;

  CycleByIndexDiff(const IndexIter& index_it) : _id_it(index_it) {}
  
  template <typename IT>
  CycleByIndexDiff(const CycleByIndexDiff<IT, Index>& other) : _id_it(other.base()) {}

  template <int ID>
  operator CycleByIndexDiff<IndexIter, ID>() const { return _id_it; }

  const IndexIter& base() const { return _id_it; }
  difference_type value() const { return *_id_it; }

public:
 
  void advance_alone() { _id_it += *_id_it; }

  template <typename IT>
  // template <typename IT, typename = std::enable_if_t<is_iterator_v<IT>>>
  void advance(IT& it) {
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, IC<IT>>);
    it += *_id_it;
    _id_it += *_id_it;
  }

};*/

/*template <typename IndexIter, int Index = 0>
class CycleByEqualNext {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;
  // index_iterator
  // lookup_iterator
  // NOTE: You need to track the current index

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using policy_category = nonfixed_jump_policy_tag;

  template <int ID>
  using indexed = CycleByEqualNext<IndexIter, ID>;

  CycleByEqualNext(const IndexIter& index_it) : _id_it(index_it) {}
  
  template <typename IT>
  CycleByEqualNext(const CycleByEqualNext<IT, Index>& other) : _id_it(other.base()) {}

  template <int ID>
  operator CycleByEqualNext<IndexIter, ID>() const { return _id_it; }

  const IndexIter& base() const { return _id_it; }
  difference_type value() const { return *_id_it; }

public:
 
  // void advance_alone() { _id_it += *_id_it; }

  template <typename T>
  void advance(index_iterator<T>& it) {
    // it += *_id_it;
    // _id_it += *_id_it;
   
    auto it0 = it;
    it = index_iterator(*_id_it);
    _id_it += *_id_it - *it0;
    
  }

};*/


template <typename IndexIter, int Index = 0>
class JumpByLookup {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<IndexIter>::iterator_category>);
  static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using policy_category = nonfixed_jump_policy_tag;

  template <int ID>
  using indexed = JumpByLookup<IndexIter, ID>;

  JumpByLookup(const IndexIter& index_it) : _id_it(index_it) {}
  
  template <typename IT>
  JumpByLookup(const JumpByLookup<IT, Index>& other) : _id_it(other.base()) {}

  template <int ID>
  operator JumpByLookup<IndexIter, ID>() const { return _id_it; }

  const IndexIter& base() const { return _id_it; }
  difference_type value() const { return *_id_it; }

public:
 
  // void advance_alone() { _id_it += *_id_it; }

  template <typename T>
  void advance(index_iterator<T>& it) const {
    it = index_iterator(_id_it[*it]);
  }

};

template <std::ptrdiff_t EQUAL_ID, int Index = 0>
class JumpByEqualStatic {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

public:
  
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using policy_category = static_jump_policy_tag;

  template <int ID>
  using indexed = JumpByEqualStatic<EQUAL_ID, ID>;
  JumpByEqualStatic() = default;

  template <int ID>
  operator JumpByEqualStatic<EQUAL_ID, ID>() const { return JumpByEqualStatic<EQUAL_ID, ID>(); }

  constexpr std::ptrdiff_t base() const { return EQUAL_ID; }
  constexpr difference_type value() const { return EQUAL_ID; }

public:

  template <typename T>
  void advance(index_iterator<T>& it) {
    static_assert(std::is_integral_v<T>);
    it = index_iterator<T>(EQUAL_ID);
  }

};



template <typename ValKey = size_t>
class KeyCyclicMatPolicy {

  // template <typename KeyCont>
  // using cont_to_key_type = typename KeyCont::key_type;
  // 
  // template <typename KeyCont>
  // using cont_to_key_iter = typename KeyCont::key_iterator;

public:

  static_assert(is_key_handle_v<ValKey>);
  static_assert(std::is_same_v<index_handle_category,
    typename handle_traits<ValKey>::handle_category>);

  using ranges_vec_type = std::vector<ValKey>;
  // using row_ranges_cont = std::vector<key_type>;

  using flat_key_type = ValKey;
  // using flat_key_iterator = typename handle_traits<ValKey>::iterator;
  using flat_key_iterator = jump_iterator<typename handle_traits<ValKey>::iterator, JumpByLookup<typename ranges_vec_type::const_iterator>>;

  using difference_type = typename handle_traits<ValKey>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  
  using key_type = size_t;
  using key_iterator = index_iterator<size_t>;
  using value_type = std::vector<key_type>;

  using reference = TensorIterable<jump_iterator<flat_key_iterator, JumpByIndexDiff<typename ranges_vec_type::const_iterator>>>;
  using pointer = reference*;
  using iterator = jump_iterator<IterableToIterator<reference>, JumpByEqualNext<typename ranges_vec_type::const_iterator>>;
  
protected:

  // void shift(key_iterator key_it, difference_type offset) {
  //   auto it = _row_ranges.begin() + *key_it;
  //   ++it;
  //   for(; it != _row_ranges.end(); ++it)
  //     *it += offset;
  // }

public:

  // KeyCyclicMatPolicy(
  //   size_type num_of_rows,
  //   size_type num_of_cols,
  //   const flat_value_type& val = flat_value_type()
  // ) : _vec(num_of_rows * num_of_cols, val) {
  //   for(size_type i = 0; i < num_of_rows; ++i) {
  //     _row_to_vec.push_back(i * num_of_cols);
  //     _next_vec_in_row.insert(_next_vec_in_row.end(), index_iterator(i * num_of_cols), index_iterator((i + 1) * num_of_cols));
  //     _next_vec_in_row.back() = -1;
  //   }
  // }
  
  void reserve(size_type n) {
    _row_begins.reserve(n);
  }

  void flat_reserve(size_type n) {
    _next_in_row.reserve(n);
  }

  size_type size() const {
    return _row_begins.size();
  }
  
  size_type flat_size() const {
    return _next_in_row.size();
  }
  
  void clear() {
    _row_begins.clear();
    _next_in_row.clear();
  }

  key_iterator begin() const {
    return key_iterator(size_type(0));
  }
  
  key_iterator end() const {
    return key_iterator(size_type(size()));
  }

  // template <typename flat_key_iterator>
  // reference<flat_key_iterator> at(key_type key) const {
  //   return reference(flat_key_iterator(key * _n), _n);
  // }
  
  reference at(key_type key) const {
    // return reference<KeyIter>(
    //   KeyIter(_row_ranges[key]),
    //   _row_ranges.cbegin() + key
    // );
    iterator it(0ULL, _row_ranges.cbegin());
    return *(it + key);
  }

  key_iterator insert(key_iterator key_it) {
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, *it);
    return key_it;
  }

  key_iterator insert(key_iterator key_it, size_type num_of_rows) {
    auto it = _row_ranges.begin() + *key_it;
    _row_ranges.insert(it, num_of_rows, *it);
    return key_it;
  }

  key_iterator erase(key_iterator key_it) {
    // auto row_ids = at(*key_it);
    // erase_in(key_it, row_ids.begin(), row_ids.end());
    shift(key_it, -at(*key_it).size());
    _row_ranges.erase(_row_ranges.begin() + *key_it);
    return key_it;
  }

  key_iterator erase(key_iterator from, key_iterator to) {
    auto itb = Iterable(
      _row_ranges.begin() + *from,
      _row_ranges.begin() + *to
    );
    shift(to, *itb.end() - *itb.begin());
    _row_ranges.erase(itb.begin(), itb.end());
    return from;
  }

  void insert_in(key_iterator key_it, flat_key_iterator flat_key) {
  // flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_key) {
    shift(key_it, 1);
    // return flat_key;
  }

  void insert_in(key_iterator key_it, flat_key_iterator flat_key, size_type num_of_vals) {
  // flat_key_iterator insert_in(key_iterator key_it, flat_key_iterator flat_key, size_type num_of_vals) {
    shift(key_it, num_of_vals);
    // return flat_key;
  }

  void erase_in(key_iterator key_it, flat_key_iterator flat_key) {
  // flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_key) {
    shift(key_it, -1);
    // return flat_key;
  }

  void erase_in(key_iterator key_it, flat_key_iterator flat_from, flat_key_iterator flat_to) {
  // flat_key_iterator erase_in(key_iterator key_it, flat_key_iterator flat_from, flat_key_iterator flat_to) {
    shift(key_it, -std::distance(flat_from, flat_to));
    // return flat_from;
  }

private:

  ranges_vec_type _row_begins;
  ranges_vec_type _next_in_row;

};



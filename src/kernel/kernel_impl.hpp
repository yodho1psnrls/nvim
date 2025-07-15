#pragma once

// NOTE: Here we added the auto update at insert/erase

#include "../meta_func.hpp"
// #include "../utilities.hpp"

#include "../containers/key_traits.hpp"
#include "../containers/arrange_traits.hpp"

// #include "../containers/tagged_tuple.hpp"
#include "../tagged.hpp"


// TODO: Here it should iterate all groups and update values of tuples and/or matrices
// that are non-const key handles
/*template <typename Cont, typename KeyIter>
inline void __update_all(
  Cont& c,
  KeyIter key_it,
  typename std::iterator_traits<KeyIter>::difference_type n = 1
) {
  using KeyType = typename std::iterator_traits<KeyIter>::value_type;
  using Traits = key_cont_traits<Cont>;
  static constexpr size_t ID = tuple_find_index_v<KeyType, typename Traits::value_type>;

  if constexpr (ID != -1) {
    static constexpr bool IS_CONST = std::is_const_v<std::remove_reference_t<std::tuple_element_t<ID, typename Traits::reference>>>;
    if constexpr (!IS_CONST) {
      // NOTE: Here you update the keys
      auto&& vals = unzipped<ID>(Traits::values(c));
      arrange_traits<Cont>::will_insert(vals.begin(), vals.end(), *key_it, n);
    }
  }
};*/


template <typename ...TaggedContainers>
class KeyKernelImpl;

template <>
struct KeyKernelImpl<> {
  using tuple_type = std::tuple<>;
  using tag_types = type_list<>;
  using key_types = type_list<>;
  using key_iterators = type_list<>;
};


// NOTE: Uses KeyMultContainer as KeyContainers
// template <typename ...KeyTypes>
template <typename ...Containers, typename ...Tags>
class KeyKernelImpl<TaggedType<Containers, Tags>...> {
// protected:

  static_assert((is_tag_type_v<Tags> && ...));
  static_assert((is_container_v<Containers> && ...));
  static_assert((is_std_tuple_v<typename key_cont_traits<Containers>::value_type> && ...));

  // vertices / edges / faces / half-edges / buckets / AABBs / ...
  // std::tuple<KeyContainers...> _groups; // layers

public:
  
  // using TupType = tagged_tuple<std::pair<Tags, Containers>...>;
  using tuple_type = std::tuple<Containers...>;

private:

  tuple_type _groups; // layers

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

protected:

  template <size_t ID>
  using id_to_raw_key_type = typename key_cont_traits<nth_type_t<ID, Containers...>>::key_type;
  
  template <size_t ID>
  using id_to_raw_key_iterator = typename key_cont_traits<nth_type_t<ID, Containers...>>::key_iterator;
  
  template <size_t ID>
  using id_to_raw_iterator = typename key_cont_traits<nth_type_t<ID, Containers...>>::value_iterator;
  
  template <size_t ID>
  using id_to_raw_const_iterator = typename key_cont_traits<nth_type_t<ID, Containers...>>::value_const_iterator;

// protected:
public:

  template <size_t ID>
  using id_to_tag = nth_type_t<ID, Tags...>;
  
  template <size_t ID>
  using id_to_key_type = TaggedType<id_to_raw_key_type<ID>, id_to_tag<ID>>;
  
  template <size_t ID>
  using id_to_key_iterator = TaggedIterator<id_to_raw_key_iterator<ID>, id_to_tag<ID>>;
  
  template <size_t ID>
  using id_to_iterator = TaggedIterator<id_to_raw_iterator<ID>, id_to_tag<ID>>;
  
  template <size_t ID>
  using id_to_const_iterator = TaggedIterator<id_to_raw_const_iterator<ID>, id_to_tag<ID>>;

  template <size_t ID>
  using id_to_cont_type = nth_type_t<ID, Containers...>;

  template <size_t ID>
  using id_to_value_type = typename key_cont_traits<id_to_cont_type<ID>>::value_type;
  
  template <size_t ID>
  using id_to_reference = typename key_cont_traits<id_to_cont_type<ID>>::reference;
  
  template <size_t ID>
  using id_to_const_reference = typename key_cont_traits<id_to_cont_type<ID>>::const_reference;
  
  // template <size_t ID>
  // using id_to_rvalue_reference = typename key_cont_traits<id_to_cont_type<ID>>::rvalue_reference;


  template <typename Tag>
  // BUG: HERE YOU HAD IT AS BOOLEAN TYPE, AND YOU GOT MASSIVE BUG !
  static constexpr size_t tag_to_id = find_index_v<get_tag_type_t<Tag>, Tags...>;
  // static constexpr bool tag_to_id = find_index_v<Tag, Tags...>;
  
  template <typename Tag>
  using tag_to_key_type = id_to_key_type<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_key_iterator = id_to_key_iterator<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_iterator = id_to_iterator<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_const_iterator = id_to_const_iterator<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_cont_type = id_to_cont_type<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_value_type = id_to_value_type<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_reference = id_to_reference<tag_to_id<Tag>>;
  
  template <typename Tag>
  using tag_to_const_reference = id_to_const_reference<tag_to_id<Tag>>;
 

  // NOTE: No need for this, since the tag_to_id works given both tags, tagged type and tagged iterator types
  // template <typename TagKey>
  // using key_to_iter_type = id_to_key_iterator<tag_to_id<get_tag_type_t<TagKey>>>;

  // template <typename Key, typename Val>
  // using kvp_to_cont_type = key_to_cont_type<Key>::template value_to_cont_type<Val>;
  // template <typename Key, size_t ValID>
  // using kvp_id_to_cont_type = key_to_cont_type<Key>::template id_to_cont_type<ValID>;
  
  using tag_types = type_list<Tags...>;
  using key_types = type_list<TaggedType<typename key_cont_traits<Containers>::key_type, Tags>...>;
  using key_iterators = type_list<TaggedIterator<typename key_cont_traits<Containers>::key_iterator, Tags>...>;
  
  template <typename Tag>
  static constexpr bool contains_tag = contains_type_v<get_tag_type_t<Tag>, Tags...>;
 
  template <typename Key>
  static constexpr bool contains_key_type = tuple_contains_type_v<Key, key_types>;
  // static constexpr bool contains_key_type = contains_type_by_v<__get_key_type, Key, KeyContainers...>;
  
  template <typename KeyIter>
  static constexpr bool contains_key_iterator = tuple_contains_type_v<KeyIter, key_iterators>;
  
  template <size_t ID>
  static constexpr bool contains_tag_id = ID < sizeof...(Containers);
  
  // template <typename Key, typename Val>
  // static constexpr bool contains_kvp = contains_key<Key> && key_to_cont_type<Key>::template contains_value<Val>;

  template <typename Tag>
  tag_to_cont_type<Tag>& group() {
    return std::get<tag_to_id<Tag>>(_groups);
  }

  template <typename Tag>
  const tag_to_cont_type<Tag>& group() const {
    return std::get<tag_to_id<Tag>>(_groups);
  }
  
  template <size_t ID>
  id_to_cont_type<ID>& group() {
    return std::get<ID>(_groups);
  }

  template <size_t ID>
  const id_to_cont_type<ID>& group() const {
    return std::get<ID>(_groups);
  }


  /*template <typename Key, typename Val>
  auto& get() {
    return get<Key>().template sub<Val>();
  }
  
  template <typename Key, typename Val>
  const auto& get() const {
    return get<Key>().template sub<Val>();
  }
  
  template <typename Key, size_t ValID>
  auto& get() {
    return get<Key>().template sub<ValID>();
  }
  
  template <typename Key, size_t ValID>
  const auto& get() const {
    return get<Key>().template sub<ValID>();
  }*/


public:

  // template <typename Args>
  // KeyKernelImpl(Args...);

  // template <typename Arg0, typename ...Arg1toN>
  // KeyKernelImpl()

  // KeyKernelImpl() : _groups(std::make_tuple(std::))


  template <bool CONDITION = (std::is_default_constructible_v<Containers> && ...),
    typename = std::enable_if_t<CONDITION>>
  KeyKernelImpl() {}

  // template <bool COND = sizeof...(Tags) != 0, typename = std::enable_if_t<COND>>
  KeyKernelImpl(const Containers&... key_conts)
  : _groups(key_conts...) {}
 
  // template <bool COND = sizeof...(Tags) != 0, typename = std::enable_if_t<COND>>
  KeyKernelImpl(Containers&&... key_conts)
  : _groups(std::move(key_conts)...) {}

  // TODO: Need to SFINAE it with can_construct_tuple_v
  // template <typename ...Args>
  // KeyKernelImpl(Args&&... args)
  // // : _groups(construct_tuple<TupType>(std::forward<Args>(args)...)) {}
  // : _groups(make_tuple_filtered<TupType>(std::forward<Args>(args)...)) {}


  template <typename Key>
  bool contains(Key key) const {
    // static_assert(contains_key_type<Key>);
    if constexpr(contains_tag<Key>)
      return group<Key>().contains(key);
    return false;
  }

  template <typename Key>
  tag_to_key_iterator<Key> find(Key key) const {
    static_assert(contains_tag<Key>);
    return group<Key>().find(key);
  }

  template <typename Tag>
  tag_to_key_iterator<Tag> begin() const {
    using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
    return tag_to_key_iterator<Tag>(Traits::begin(group<Tag>()));
  }

  template <typename Tag>
  tag_to_key_iterator<Tag> end() const {
    using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
    return tag_to_key_iterator<Tag>(Traits::end(group<Tag>()));
  }

  template <typename Tag>
  View<tag_to_key_iterator<Tag>> keys() const {
    // return View(begin<Tag>(), end<Tag>());
    using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
    return Traits::keys(group<Tag>());
  }

  template <typename Tag>
  View<tag_to_const_iterator<Tag>> values() const {
    using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
    return Traits::values(group<Tag>());
  }

  template <typename Tag>
  View<tag_to_iterator<Tag>> values() {
    using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
    return Traits::values(group<Tag>());
  }

  template <typename Tag, typename Val>
  // View<tag_to_const_iterator<Tag>>
  auto values() const {
    using Cont = tag_to_cont_type<Tag>;
    using Traits = key_cont_traits<Cont>;
    static constexpr size_t ID = tuple_find_index_v<Val, typename Cont::value_type>;

    return unzipped<ID>(Traits::values(group<Tag>()));
  }

  template <typename Tag, typename Val>
  // View<tag_to_iterator<Tag>>
  auto values() {
    using Cont = tag_to_cont_type<Tag>;
    using Traits = key_cont_traits<Cont>;
    static constexpr size_t ID = tuple_find_index_v<Val, typename Cont::value_type>;

    return unzipped<ID>(Traits::values(group<Tag>()));
  }


  template <typename Key, typename MiddleKey, typename Val>
  auto values() const {
    auto keys = values<Key, MiddleKey>();
    const auto& lookup = group<MiddleKey>();
    static constexpr size_t ID = tuple_find_index_v<Val, typename tag_to_cont_type<MiddleKey>::value_type>;

    return unzipped<ID>(View(
      make_lookup_iterator(keys.begin(), lookup),
      make_lookup_iterator(keys.end(), lookup)
    ));
  }

  template <typename Key, typename MiddleKey, typename Val>
  auto values() {
    auto keys = values<Key, MiddleKey>();
    auto& lookup = group<MiddleKey>();
    static constexpr size_t ID = tuple_find_index_v<Val, typename tag_to_cont_type<MiddleKey>::value_type>;

    return unzipped<ID>(View(
      make_lookup_iterator(keys.begin(), lookup),
      make_lookup_iterator(keys.end(), lookup)
    ));
  }

  // TODO: Generalize this recursively, with a bunch of template metaprogramming

  // template <typename KeyType, typename ValType, typename ...Rest>
  // auto values() const {
  //   if constexpr (sizeof...(Rest)) {
  //     static constexpr size_t ID = tuple_find_index_v<ValType, typename tag_to_cont_type<KeyType>::value_type>;
  //     return unzipped<ID>(group<KeyType>());
  //   }
  //   auto temp = values()
  //   return View(
  //     make_lookup_iterator()
  //   );
  // }


  template <typename Key>
  size_type size() const {
    return group<Key>().size();
  }

  template <typename Key>
  bool empty() const {
    return group<Key>().empty();
  }


  /*template <typename Key, typename Val>
  auto&& at(Key key) {
    static_assert(contains_key_type<Key>);
    return group<Key>().template at<Val>(key);
  }

  template <typename Key, typename Val>
  auto&& at(Key key) const {
    static_assert(contains_key_type<Key>);
    return group<Key>().template at<Val>(key);
  }
  
  template <typename Key, size_t ValID = 0>
  auto&& at(Key key) {
    static_assert(contains_key_type<Key>);
    return group<Key>().template at<ValID>(key);
  }

  template <typename Key, size_t ValID = 0>
  auto&& at(Key key) const {
    static_assert(contains_key_type<Key>);
    return group<Key>().template at<ValID>(key);
  }
  
  template <size_t KeyID = 0, size_t ValID = 0>
  auto&& at(id_to_key_type<KeyID> key) {
    static_assert(contains_tuple_id<KeyID>);
    return group<KeyID>().template at<ValID>(key);
  }

  template <size_t KeyID = 0, size_t ValID = 0>
  auto&& at(id_to_key_type<KeyID> key) const {
    static_assert(contains_tuple_id<KeyID>);
    return group<KeyID>().template at<ValID>(key);
  }
  
  template <size_t KeyID, typename Val>
  auto&& at(id_to_key_type<KeyID> key) {
    static_assert(contains_tuple_id<KeyID>);
    static_assert(id_to_cont_type<KeyID>::template contains_value<Val>);
    return group<KeyID>().template at<Val>(key);
  }

  template <size_t KeyID, typename Val>
  auto&& at(id_to_key_type<KeyID> key) const {
    static_assert(contains_tuple_id<KeyID>);
    static_assert(id_to_cont_type<KeyID>::template contains_value<Val>);
    return group<KeyID>().template at<Val>(key);
  }*/

  // template <typename Tag>
  // auto&& at(tag_to_key_type<Tag> key) {
  //  using Traits = key_cont_traits<tag_to_cont_type<Tag>>;
  //  return Traits::at(group<Tag>(), key);
  // }

  template <typename Key>
  tag_to_reference<Key> at(Key key) {
  // decltype(auto) at(Key key) {
    static_assert(is_tagged_type_v<Key>);
    static_assert(contains_key_type<Key>);
    using Traits = key_cont_traits<tag_to_cont_type<Key>>;
    return Traits::at(group<Key>(), key);
  }

  template <typename Key>
  tag_to_const_reference<Key> at(Key key) const {
  // decltype(auto) at(Key key) const {
    static_assert(is_tagged_type_v<Key>);
    static_assert(contains_key_type<Key>);

    using Traits = key_cont_traits<tag_to_cont_type<Key>>;
    return Traits::at(group<Key>(), key);
  }


  template <typename KeyTag>
  void push_back(const tag_to_value_type<KeyTag>& val) {
    group<KeyTag>().push_back(val);
  }

  template <typename KeyTag>
  void push_back(tag_to_value_type<KeyTag>&& val) {
    group<KeyTag>().push_back(std::move(val));
  }

  template <typename KeyTag>
  void pop_back() {
    group<KeyTag>().pop_back();
  }

  template <typename KeyIter>
  KeyIter insert(KeyIter key_it, const tag_to_value_type<KeyIter>& val) {
  // KeyIter insert(KeyIter key_it, tag_to_const_reference<KeyIter> val) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(is_tagged_type_v<KeyIter>);
    static_assert(contains_key_iterator<KeyIter>);

    using KeyType = typename std::iterator_traits<KeyIter>::value_type;
    // using KeyRef = typename std::iterator_traits<KeyIter>::reference;
    using KeyRef = std::remove_const_t<KeyType>&;
    using ContType = tag_to_cont_type<KeyIter>;
    
    // std::apply([key_it](Containers& ...cont) { (__update_all(cont, key_it), ...); }, _groups);
    for_each_rec<KeyRef>(_groups, [this, &key_it](KeyRef k) {
      arrange_traits<ContType>::will_insert(k, group<KeyIter>(), *key_it);
    });

    using Traits = key_cont_traits<ContType>;
    return KeyIter(Traits::insert(group<KeyIter>(), key_it.base(), val));
  }

  template <typename KeyIter>
  KeyIter insert(KeyIter key_it, size_type n, const tag_to_value_type<KeyIter>& val) {
  // KeyIter insert(KeyIter key_it, size_type n, tag_to_const_reference<KeyIter> val) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(is_tagged_type_v<KeyIter>);
    static_assert(contains_key_iterator<KeyIter>);
    
    using KeyType = typename std::iterator_traits<KeyIter>::value_type;
    // using KeyRef = typename std::iterator_traits<KeyIter>::reference;
    using KeyRef = std::remove_const_t<KeyType>&;
    using ContType = tag_to_cont_type<KeyIter>;
    
    for_each_rec<KeyRef>(_groups, [this, &key_it, n](KeyRef k) {
      arrange_traits<ContType>::will_insert(k, group<KeyIter>(), *key_it, n);
    });

    using Traits = key_cont_traits<tag_to_cont_type<KeyIter>>;
    return KeyIter(Traits::insert(group<KeyIter>(), key_it.base(), n, val));
  }

  template <typename KeyIter, typename ValIter, typename = std::enable_if_t<is_iterator_v<ValIter>>>
  KeyIter insert(KeyIter key_it, ValIter first, ValIter last) {
    static_assert(std::is_convertible_v<
      typename std::iterator_traits<ValIter>::value_type,
      tag_to_value_type<KeyIter>>
    );
    
    static_assert(is_iterator_v<KeyIter>);
    static_assert(is_tagged_type_v<KeyIter>);
    static_assert(contains_key_iterator<KeyIter>);
    
    using KeyType = typename std::iterator_traits<KeyIter>::value_type;
    // using KeyRef = typename std::iterator_traits<KeyIter>::reference;
    using KeyRef = std::remove_const_t<KeyType>&;
    using ContType = tag_to_cont_type<KeyIter>;

    size_type count = size<KeyIter>();
    using Traits = key_cont_traits<tag_to_cont_type<KeyIter>>;
    KeyIter kit(Traits::insert(group<KeyIter>(), key_it.base(), first, last));
    count = size<KeyIter>() - count;
    
    for_each_rec<KeyRef>(_groups, [this, &key_it, count](KeyRef k) {
      arrange_traits<ContType>::will_insert(k, group<KeyIter>(), *key_it, count);
    });

    return kit;
  }

  template <typename KeyIter>
  KeyIter erase(KeyIter key_it) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(is_tagged_type_v<KeyIter>);
    static_assert(contains_key_iterator<KeyIter>);
    
    using KeyType = typename std::iterator_traits<KeyIter>::value_type;
    // using KeyRef = typename std::iterator_traits<KeyIter>::reference;
    using KeyRef = std::remove_const_t<KeyType>&;
    using ContType = tag_to_cont_type<KeyIter>;

    using Traits = key_cont_traits<tag_to_cont_type<KeyIter>>;
    KeyIter kit(Traits::erase(group<KeyIter>(), key_it.base()));

    for_each_rec<KeyRef>(_groups, [this, &key_it](KeyRef k) {
      arrange_traits<ContType>::was_erased(k, group<KeyIter>(), *key_it);
    });

    return kit;
  }

  template <typename KeyIter>
  KeyIter erase(KeyIter from, KeyIter to) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(is_tagged_type_v<KeyIter>);
    static_assert(contains_key_iterator<KeyIter>,
      "The given key iterator type is not contained in this kernel");

    using KeyType = typename std::iterator_traits<KeyIter>::value_type;
    // using KeyRef = typename std::iterator_traits<KeyIter>::reference;
    using KeyRef = std::remove_const_t<KeyType>&;
    using ContType = tag_to_cont_type<KeyIter>;

    size_type count = size<KeyIter>();
    using Traits = key_cont_traits<ContType>;
    KeyIter kit(Traits::erase(group<KeyIter>(), from.base(), to.base()));
    count = count - size<KeyIter>();

    for_each_rec<KeyRef>(_groups, [this, &kit, count](KeyRef k) {
      arrange_traits<ContType>::was_erased(k, group<KeyIter>(), *kit, count);
    });

    return kit;
  }

};






#pragma once

#include "../iterators/view.hpp"

#include "../iterators/index_iterator.hpp"
#include "../iterators/iterator_iterator.hpp"

#include "../iterators/transform_iterator.hpp"
#include "../iterators/lookup_iterator.hpp"

#include "../utilities.hpp"


template <typename Container>
struct key_cont_traits;


// ============================================================== //

template <typename IterType>
class key_cont_traits<View<IterType>> {

  using container = View<IterType>;

public:

  using value_type = typename container::value_type;
  using reference = typename container::reference;
  using pointer = typename container::pointer;
  using value_iterator = typename container::iterator;
  
  using difference_type = typename container::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  
  using key_type = size_type;
  using key_iterator = IndexIterator<key_type>;
  
 
  static bool contains(const container& cont, key_type key) {
    size_type k(key);
    if constexpr (std::is_unsigned_v<size_type>)
      return k < cont.size();
    else
      return 0 <= k && k < cont.size();
  }

  static key_iterator find(const container&, key_type key) {
    return key_iterator(key);
  }

  static value_iterator find(const container& cont, key_iterator key_it) {
    return cont.begin() + *key_it;
  }

  template <typename KeyIter>
  static LookupIterator<KeyIter, const container>
  find(const container& cont, KeyIter key_it) {
    return make_lookup_iterator(key_it, cont);
  }

  static key_iterator begin(const container& cont) {
  // static key_iterator kbegin(const container& cont) {
    return key_iterator(key_type(0ULL));
  }

  static key_iterator end(const container& cont) {
  // static key_iterator kend(const container& cont) {
    return key_iterator(key_type(cont.size()));
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      key_iterator(key_type(0ULL)),
      key_iterator(key_type(cont.size()))
    };
  }

  static View<value_iterator> values(const container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }

  // static View<value_iterator> cvalues(const container& cont) {
  //   return {
  //     cont.cbegin(),
  //     cont.cend()
  //   };
  // }

  static reference at(const container& cont, key_type key) {
    return cont[size_type(key)];
  }

};

// template <typename ViewType>
// struct __view_key_cont_traits {
//
//   static_assert(is_range_v<ViewType>);
//
// };


template <
  typename VecType
  // typename KeyType = typename VecType::size_type
>
class __vec_key_cont_traits {
  
  static_assert(is_container_v<VecType>);
  // static_assert(is_range_v<VecType>);
  static_assert(is_vector_container_v<VecType>);

  // using container = std::vector<T, Alloc>;
  // using container = VecType;
  using container = std::remove_cvref_t<VecType>;
  
  // static_assert(std::is_convertible_v<KeyType, typename VecType::size_type>);

public:
  

  // template <typename KeyIter>
  // using lookup_iterator = TransformIterator<KeyIter, LookupFunctor<container>>;
  // 
  // template <typename KeyIter>
  // using const_lookup_iterator = TransformIterator<KeyIter, LookupFunctor<const container>>;


  using value_type = typename container::value_type;
  using reference = typename container::reference;
  using const_reference = typename container::const_reference;

  using value_iterator = typename container::iterator;
  using value_const_iterator = typename container::const_iterator;

  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;

  // template <typename Tag = void>
  // using tagged_key_type = index_handle<size_type, Tag>;
  //
  // template <typename Tag = void>
  // // using tagged_key_iterator = index_iterator<tagged_key_type<Tag>>;
  // using tagged_key_iterator = index_iterator<size_type, tagged_key_type<Tag>>;
  //  
  // using key_type = tagged_key_type<void>;
  // using key_iterator = tagged_key_iterator<void>;
  
  using key_type = size_type;
  using key_iterator = IndexIterator<key_type>;


  static bool contains(const container& cont, key_type key) {
    size_type k(key);
    if constexpr (std::is_unsigned_v<size_type>)
      return k < cont.size();
    else
      return 0 <= k && k < cont.size();
  }

  // static key_iterator find(const container& cont, key_type key) {
  //   return key_iterator(key);
  // }

  // static value_iterator find(const container& cont, key_type key) {
  //   return cont.begin() + difference_type(key);
  // }
  // static value_iterator find(container& cont, key_type key) {
  //   return cont.begin() + difference_type(key);
  // }

  static key_iterator find(const container&, key_type key) {
    return key_iterator(key);
  }

  static value_const_iterator find(const container& cont, key_iterator key_it) {
    return cont.begin() + *key_it;
  }

  static value_iterator find(container& cont, key_iterator key_it) {
    return cont.begin() + *key_it;
  }

  template <typename KeyIter, typename = std::enable_if_t<
    is_iterator_v<KeyIter> &&
    std::is_convertible_v<std::iter_reference_t<KeyIter>, key_type>>
  >
  static LookupIterator<KeyIter, const container>
  find(const container& cont, const KeyIter& key_it) {
    return make_lookup_iterator(key_it, cont);
  }

  template <typename KeyIter, typename = std::enable_if_t<
    is_iterator_v<KeyIter> &&
    std::is_convertible_v<std::iter_reference_t<KeyIter>, key_type>>
  >
  static LookupIterator<KeyIter, container>
  find(container& cont, const KeyIter& key_it) {
    return make_lookup_iterator(key_it, cont);
  }


  static key_iterator begin(const container& cont) {
  // static key_iterator kbegin(const container& cont) {
    return key_iterator(key_type(0ULL));
  }

  static key_iterator end(const container& cont) {
  // static key_iterator kend(const container& cont) {
    return key_iterator(key_type(cont.size()));
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      key_iterator(key_type(0ULL)),
      key_iterator(key_type(cont.size()))
    };
  }


  /*static value_iterator vbegin(container& cont) {
    return cont.begin();
  }

  static value_iterator vend(container& cont) {
    return cont.end();
  }
  
  static value_const_iterator vbegin(const container& cont) {
    return cont.begin();
  }

  static value_const_iterator vend(const container& cont) {
    return cont.end();
  }
  
  static value_const_iterator cvbegin(const container& cont) {
    return cont.begin();
  }

  static value_const_iterator cvend(const container& cont) {
    return cont.end();
  }*/

  static View<value_iterator> values(container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }

  static View<value_const_iterator> values(const container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }

  static View<value_const_iterator> cvalues(const container& cont) {
    return {
      cont.cbegin(),
      cont.cend()
    };
  }


  static reference at(container& cont, key_type key) {
    return cont[size_type(key)];
  }

  static const_reference at(const container& cont, key_type key) {
    return cont[size_type(key)];
  }

  // =============================== KEY BASED RESIZE METHODS ================================== //

  static key_iterator insert(container& cont, key_iterator key_it, const value_type& val = value_type()) {
    cont.insert(cont.begin() + size_type(*key_it), val);
    return key_it;
  }

  static key_iterator insert(container& cont, key_iterator key_it, size_type n, const value_type& val = value_type()) {
    // cont.insert(
    //   cont.begin() + *key_it,
    //   DefaultAssignIterator<value_type>(0),
    //   DefaultAssignIterator<value_type>(n)
    // );
    cont.insert(cont.begin() + size_type(*key_it), n, val);
    return key_it;
  }

  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  static key_iterator insert(container& cont, key_iterator key_it, IterType first, IterType last) {
    static_assert(std::is_convertible_v<
      typename std::iterator_traits<IterType>::value_type,
      value_type>
    );
    cont.insert(cont.begin() + size_type(*key_it), first, last);
    return key_it;
  }

  static key_iterator erase(container& cont, key_iterator key_it) {
    cont.erase(cont.begin() + size_type(*key_it));
    return key_it;
  }

  static key_iterator erase(container& cont, key_iterator from, key_iterator to) {
    cont.erase(
      cont.begin() + size_type(*from),
      cont.begin() + size_type(*to)
    );
    return from;
  }


  // =============================== VALUE BASED RESIZE METHODS ================================== //

  /*static value_iterator insert(container& cont, value_iterator it, const value_type& val = value_type()) {
    return cont.insert(it, val);
  }
  
  static value_iterator insert(container& cont, value_iterator it, size_type n, const value_type& val = value_type()) {
    return cont.insert(it, n, val);
  }
  
  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  static value_iterator insert(container& cont, value_iterator it, IterType first, IterType last) {
    static_assert(std::is_convertible_v<
      typename std::iterator_traits<IterType>::value_type,
      value_type>
    );
    return cont.insert(it, first, last);
  }
  
  static value_iterator erase(container& cont, value_iterator it) {
    return cont.erase(it);
  }
  
  static value_iterator erase(container& cont, value_iterator from, value_iterator to) {
    return cont.erase(from, to);
  }*/

};


template <typename Container>
class __stability_of_ref_key_cont_traits {
  
  static_assert(is_container_v<Container>);
  // static_assert(is_range_v<Container>);

  // using container = std::vector<T, Alloc>;
  // using container = Container;
  using container = std::remove_cvref_t<Container>;

public:

  template <typename KeyIter>
  using lookup_iterator = TransformIterator<KeyIter, DereferenceFunctor>;
  
  using value_type = typename container::value_type;
  using reference = typename container::reference;
  using const_reference = typename container::const_reference;

  using value_iterator = typename container::iterator;
  using value_const_iterator = typename container::const_iterator;

  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;

  // using key_type = typename container::pointer;
  // using key_type = typename container::iterator;
  using key_type = typename container::const_iterator;
  using key_iterator = IteratorIterator<key_type>;
  
  // template <typename Tag>
  // using tagged_key_type = iter_handle<Tag, key_type>;
  //
  // template <typename Tag>
  // using tagged_key_iterator = iterator_iterator<tagged_key_type<Tag>>;


  static bool contains(const container& cont, key_type key) {
    return key != cont.end();
  }


  // static key_iterator find(const container& cont, key_type key) {
  //   return key_iterator(key);
  // }

  // static value_iterator find(container& cont, key_type key) {
  //   return cont.begin() + difference_type(key);
  // }
  // static value_iterator find(const container& cont, key_type key) {
  //   return cont.begin() + difference_type(key);
  // }

  static key_iterator find(const container&, key_type key) {
    return key_iterator(key);
  }

  static value_const_iterator find(const container&, key_iterator key_it) {
    return key_it.base();
  }
  
  static value_iterator find(container& cont, key_iterator key_it) {
    return cont.erase(key_it.base(), key_it.base());
  }

  template <typename KeyIter>
  static TransformIterator<KeyIter, DereferenceFunctor>
  find(const container&, KeyIter key_it) {
    static_assert(std::is_convertible_v<std::iter_reference_t<std::iter_value_t<KeyIter>>, const_reference>);
    static_assert(is_const_iterator_v<KeyIter>);
    // return inverse_iterator_iterator(key_it);
    return TransfromIterator(key_it, DereferenceFunctor());
  }

  template <typename KeyIter>
  static TransformIterator<KeyIter, DereferenceFunctor>
  find(container&, KeyIter key_it) {
    static_assert(std::is_convertible_v<std::iter_reference_t<std::iter_value_t<KeyIter>>, reference>);
    // return inverse_iterator_iterator(key_it);
    return TransfromIterator(key_it, DereferenceFunctor());
  }


  static key_iterator begin(const container& cont) {
    return key_iterator(cont.begin());
  }

  static key_iterator end(const container& cont) {
    return key_iterator(cont.end());
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      key_iterator(cont.begin()),
      key_iterator(cont.end())
    };
  }
  
  static View<value_iterator> values(container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }
  
  static View<value_const_iterator> values(const container& cont) {
    return {
      cont.cbegin(),
      cont.cend()
    };
  }

  static reference at(container& cont, key_type key) {
    // return *key;
    // return const_cast(*key);
    return *cont.erase(key, key); // empty erase, to get non-conts iterator
  }

  static const_reference at(const container& cont, key_type key) {
    return *key;
  }
  
  static key_iterator insert(container& cont, key_iterator key_it, const value_type& val = value_type()) {
    return key_iterator(cont.insert(*key_it, val));
  }
  
  static key_iterator insert(container& cont, key_iterator key_it, size_type n, const value_type& val = value_type()) {
    // return key_iterator(cont.insert(
    //   *key_it,
    //   DefaultAssignIterator<value_type>(0),
    //   DefaultAssignIterator<value_type>(n)
    // ));
    return key_iterator(cont.insert(*key_it, n, val));
  }
  
  template <typename ValIter, typename = std::enable_if_t<is_iterator_v<ValIter>>>
  static key_iterator insert(container& cont, key_iterator key_it, ValIter first, ValIter last) {
    return key_iterator(cont.insert(*key_it, first, last));
  }
  
  static key_iterator erase(container& cont, key_iterator key_it) {
    return key_iterator(cont.erase(*key_it));
  }
  
  static key_iterator erase(container& cont, key_iterator from, key_iterator to) {
    return key_iterator(cont.erase(*from, *to));
  }

};


template <typename SetType>
class __set_key_cont_traits {
  
  static_assert(is_container_v<SetType>);
  static_assert(is_set_container_v<SetType>);

  // using container = std::vector<T, Alloc>;
  // using container = SetType;
  using container = std::remove_cvref_t<SetType>;

public:
  
  template <typename KeyIter>
  using lookup_iterator = KeyIter;


  using key_type = typename container::key_type;
  using key_iterator = typename container::const_iterator;
  
  using value_type = typename container::value_type;
  using const_reference = typename container::const_reference;
  using reference = const_reference;

  using value_iterator = typename container::iterator;
  using value_const_iterator = typename container::const_iterator;

  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;


  static bool contains(const container& cont, key_type key) {
    return cont.contains(key);
  }
 

  // static value_iterator find(const container& cont, key_type key) {
  //   return cont.find(key);
  // }

  static key_iterator find(const container& cont, key_type key) {
    return cont.find(key);
  }

  static value_const_iterator find(const container&, key_iterator key_it) {
    return key_it;
  }
  
  static value_iterator find(container& cont, key_iterator key_it) {
    return cont.erase(key_it, key_it);
  }

  template <typename KeyIter>
  static KeyIter find(const container&, KeyIter key_it) {
    static_assert(std::is_convertible_v<std::iter_reference_t<KeyIter>, const_reference>);
    static_assert(is_const_iterator_v<KeyIter>);
    return key_it;
  }

  template <typename KeyIter>
  static KeyIter find(container&, KeyIter key_it) {
    static_assert(std::is_convertible_v<std::iter_reference_t<KeyIter>, reference>);
    static_assert(!is_const_iterator_v<KeyIter>);
    return key_it;
  }


  static key_iterator begin(const container& cont) {
    return key_iterator(cont.begin());
  }

  static key_iterator end(const container& cont) {
    return key_iterator(cont.end());
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      key_iterator(cont.begin()),
      key_iterator(cont.end())
    };
  }
  
  static View<value_iterator> values(container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }
  
  static View<value_const_iterator> values(const container& cont) {
    return {
      cont.cbegin(),
      cont.cend()
    };
  }

  static reference at(container& cont, key_type key) {
    return key;
  }

  static const_reference at(const container& cont, key_type key) {
    return key;
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it) {
    static_assert(!std::is_same_v<KeyIter, key_iterator>);
    cont.insert(*key_it);
    return key_it;
    // return cont.insert(*key_it).first;
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, size_type n) {
    static_assert(!std::is_same_v<KeyIter, key_iterator>);
    cont.insert(key_it, std::next(key_it, n));
    return key_it;
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter erase(container& cont, KeyIter key_it) {
    if constexpr (std::is_same_v<KeyIter, key_iterator>)
      return cont.erase(key_it);
    else {
      cont.erase(*key_it);
      return key_it;
    }
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter erase(container& cont, KeyIter from, KeyIter to) {
    if constexpr (std::is_same_v<KeyIter, key_iterator>)
      return cont.erase(from, to);
    else {
      KeyIter temp(from);
      while(from != to) {
        // cont.erase(cont.find(*from));
        cont.erase(*from);
        ++from;
      }
      return temp;
    }
  }

};


template <typename MapType>
class __map_key_cont_traits {

  static_assert(is_container_v<MapType>);
  static_assert(is_map_container_v<MapType>);

  // using container = MapType;
  using container = std::remove_cvref_t<MapType>;

public:
  
  // template <typename KeyIter>
  // using lookup_iterator = TransformIterator<KeyIter, LookupFunctor<container>>;
  // 
  // template <typename KeyIter>
  // using const_lookup_iterator = TransformIterator<KeyIter, LookupFunctor<const container>>;


  using key_type = typename MapType::key_type;
  using key_iterator = TransformIterator<typename MapType::const_iterator, PairToFirst<typename MapType::value_type>>;
  
  using value_type = typename container::mapped_type;
  using reference = value_type&;
  using const_reference = const value_type&;

  using value_iterator = TransformIterator<typename MapType::iterator, PairToSecond<typename MapType::value_type>>;
  using value_const_iterator = TransformIterator<typename MapType::const_iterator, PairToSecond<typename MapType::value_type>>;

  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;


  static bool contains(const container& cont, key_type key) {
    return cont.contains(key);
  }
 

  /*static value_iterator find(const container& cont, const key_type& key) {
    return value_iterator(cont.find(key));
  }
  static value_iterator find(container& cont, const key_type& key) {
    return value_iterator(cont.find(key));
  }*/

  // static key_iterator find(const container& cont, key_type key) {
  //   return key_iterator(cont.find(key));
  // }
  
  static key_iterator find(const container& cont, key_type key) {
    return key_iterator(cont.find(key));
  }

  static value_const_iterator find(const container&, key_iterator key_it) {
    return value_iterator(key_it.base());
  }
  
  static value_iterator find(container& cont, key_iterator key_it) {
    return value_iterator(cont.erase(key_it.base(), key_it.base()));
  }
  
  template <typename KeyIter>
  static LookupIterator<KeyIter, const container>
  find(const container& cont, KeyIter key_it) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(std::is_convertible_v<std::iter_reference_t<KeyIter>, key_type>);
    return make_lookup_iterator(key_it, cont);
  }

  template <typename KeyIter>
  static LookupIterator<KeyIter, container>
  find(container& cont, KeyIter key_it) {
    static_assert(is_iterator_v<KeyIter>);
    static_assert(std::is_convertible_v<std::iter_reference_t<KeyIter>, key_type>);
    return make_lookup_iterator(key_it, cont);
  }


  static key_iterator begin(const container& cont) {
    return key_iterator(cont.begin());
  }

  static key_iterator end(const container& cont) {
    return key_iterator(cont.end());
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      key_iterator(cont.begin()),
      key_iterator(cont.end())
    };
  }
  
  static View<value_iterator> values(container& cont) {
    return {
      value_iterator(cont.begin()),
      value_iterator(cont.end())
    };
  }
  
  static View<value_const_iterator> values(const container& cont) {
    return {
      value_const_iterator(cont.cbegin()),
      value_const_iterator(cont.cend())
    };
  }

  static reference at(container& cont, key_type key) {
    return cont[key];
  }

  static const_reference at(const container& cont, key_type key) {
    return cont.at(key);
  }

  // NOTE: !!! DONT EVER INSERT USING END OF THE EXACT SAME MAP,
  // THIS MAP TRAITS EXPECTS A KEY ITERATOR THAT IS LIKE A GENERATOR
  // IF YOU INSERT END KEY, IT WILL JUST INSERT THE NEW VALUE AT A RANDOM KEY
  // WHICH IS UNDEFINED BEHAIVIOUR !!! OR IT WILL INSERT OVER AN ALREADY USED KEYS

  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, const value_type& val = value_type()) {
    // Commented out, because it may be the same type container, but other instance
    // static_assert(!std::is_same_v<KeyIter, key_iterator>);
    cont.insert({*key_it, val});
    return key_it;
    // return cont.insert({*key_it, val});
  }
  
  /*template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, size_type n) {
    // Commented out, because it may be the same type container, but other instance
    // static_assert(!std::is_same_v<KeyIter, key_iterator>);
    MapDefaultAssignIterator<KeyIter, value_type> kvp_it(key_it);
    cont.insert(kvp_it, std::next(kvp_it, n));
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, size_type n, const value_type& val) {
    // Commented out, because it may be the same type container, but other instance
    // static_assert(!std::is_same_v<KeyIter, key_iterator>);
    MapValueAssignIterator<KeyIter, value_type> kvp_it(key_it, val);
    cont.insert(kvp_it, std::next(kvp_it, n));
    return key_it;
  }*/
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, size_type n, const value_type& val = value_type()) {
    // Commented out, because it may be the same type container, but other instance
    // static_assert(!std::is_same_v<KeyIter, key_iterator>);

    if constexpr (requires(container& cont, size_type n){cont.reserve(n);})
      cont.reserve(n);

    while(n--)
      cont.insert({*(key_it++), val});

    return key_it;
  }
  
  template <typename KeyIter, typename ValIter, typename =
    std::enable_if_t<is_iterator_v<KeyIter> && is_iterator_v<ValIter>>>
  static KeyIter insert(container& cont, KeyIter key_it, ValIter first, ValIter last) {
    if constexpr (
      requires(container& cont, size_type n){cont.reserve(n);}
      && std::is_base_of_v<
        std::random_access_iterator_tag,
        typename std::iterator_traits<ValIter>::iterator_category>
    ) cont.reserve(std::distance(first, last));

    while(first != last)
      cont.insert({*(key_it++), *(first++)});

    return key_it;
  }


  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter erase(container& cont, KeyIter key_it) {
    if constexpr (std::is_same_v<KeyIter, key_iterator>)
      return cont.erase(key_it.base());
    else {
      cont.erase(*key_it);
      return key_it;
    }
  }
  
  template <typename KeyIter, typename = std::enable_if_t<is_iterator_v<KeyIter>>>
  static KeyIter erase(container& cont, KeyIter from, KeyIter to) {
    if constexpr (std::is_same_v<KeyIter, key_iterator>)
      return cont.erase(from.base(), to.base());
    else {
      KeyIter temp(from);
      while(from != to) {
        // cont.erase(cont.find(*from));
        cont.erase(*from);
        ++from;
      }
      return temp;
    }
  }

};


// ============================================================== //


/*template <typename Container>
struct key_cont_traits : public _if_t<
  is_set_container_v<Container>,
  __set_key_cont_traits<Container>,
  _if_t<
    is_map_container_v<Container>,
    __map_key_cont_traits<Container>,
    _if_t<
      is_vector_container_v<Container>,
      __vec_key_cont_traits<Container>,
      __stability_of_ref_key_cont_traits<Container>
    >
  >
> {
  static_assert(is_container_v<Container>);
};*/

template <typename Container>
struct key_cont_traits : public _if_t<
  // !is_container_v<Container>,
  !is_range_v<Container>,
  EmptyType,
  _if_t<
    is_set_container_v<Container>,
    __set_key_cont_traits<Container>,
    _if_t<
      is_map_container_v<Container>,
      __map_key_cont_traits<Container>,
      _if_t<
        is_vector_container_v<Container>,
        __vec_key_cont_traits<Container>,
        __stability_of_ref_key_cont_traits<Container>
        // _if_t<
        //  is_container_v<Container>,
        //  __stability_of_ref_key_cont_traits<Container>,
        //  EmptyType
        // >
      >
    >
  >
> {};


template <typename Container, typename = void>
struct has_cont_traits
: public std::false_type {};

template <typename Container>
struct has_cont_traits<
  Container,
  std::void_t<
    typename key_cont_traits<Container>::key_type,
    typename key_cont_traits<Container>::value_type,
    typename key_cont_traits<Container>::key_iterator,
    typename key_cont_traits<Container>::value_iterator
  >
> : public std::true_type {};

template <typename Container>
constexpr bool has_cont_traits_v = has_cont_traits<Container>::value;


template <typename Container, typename = void>
struct is_redundant_lookup
: public std::false_type {};

template <typename Container>
class is_redundant_lookup<
  Container,
  std::void_t<
    typename key_cont_traits<Container>::key_type,
    typename key_cont_traits<Container>::value_type,
    typename key_cont_traits<Container>::value_const_iterator
  >
> {
  // using key_iterator = typename key_cont_traits<Container>::key_iterator;
  // using value_const_iterator = typename key_cont_traits<Container>::value_const_iterator;
  using key_type = typename key_cont_traits<Container>::key_type;
  using value_type = typename key_cont_traits<Container>::value_type;
  using value_const_iterator = typename key_cont_traits<Container>::value_const_iterator;
public:
  static constexpr bool value =
    // std::is_same_v<key_iterator, value_const_iterator> ||  // in set types
    // std::is_same_v<key_iterator, IteratorIterator<value_const_iterator>>;  // in list types
    std::is_same_v<key_type, value_type> || // in set types
    std::is_same_v<key_type, value_const_iterator>; // in list types
};


template <typename Container>
constexpr bool is_redundant_lookup_v = is_redundant_lookup<Container>::value;


// =================================================================================== //



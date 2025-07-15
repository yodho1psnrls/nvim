#pragma once

#include "../meta_func.hpp"
#include "../containers/key_traits.hpp"
#include "../iterators/lookup_iterator.hpp"

#include "../iterators/zip_iterator.hpp"
#include "../iterators/unzip_iterator.hpp"

#include <tuple>
#include <type_traits>
#include <iterator>


template <typename ...Containers>
class PropertyContainer;

template <size_t ID, typename KeyIter, typename ...Containers>
// inline auto make_unzip_iterator(const LookupIterator<KeyIter, PropertyContainer<Containers...>>& it) {
inline auto
// inline TransformIterator<KeyIter, ContainerLookupFunctor<nth_type_t<ID, Containers...>>>
make_unzip_iterator(const TransformIterator<KeyIter, ContainerLookupFunctor<PropertyContainer<Containers...>>>& it) {
  return make_lookup_iterator(it.base(), it.functor().lookup().template sub<ID>());
}

template <size_t ID, typename KeyIter, typename ...Containers>
inline auto
// inline TransformIterator<KeyIter, ContainerLookupFunctor<const nth_type_t<ID, Containers...>>>
make_unzip_iterator(const TransformIterator<KeyIter, ContainerLookupFunctor<const PropertyContainer<Containers...>>>& it) {
  return make_lookup_iterator(it.base(), it.functor().lookup().template sub<ID>());
}

// TEST:
// IndexIterator ci('A');
// IndexIterator ii(0);
// using cont_type = PropertyContainer<std::vector<int>, std::vector<char>>;
// cont_type cont({ii, ii+5}, {ci, ci+5});
// auto view = unzipped<0>(cont);
// static_assert(std::is_same_v<
//   UnzipIterator<cont_type::const_iterator, 1>,
//   LookupIterator<IndexIterator<size_t>, const std::vector<char>>
// >);



/*template <typename Container>
struct __key_cont_to_value {
  using type = typename key_cont_traits<Container>::value_type;
};

template <typename Container>
using __key_cont_to_value_t = typename __key_cont_to_value<Container>::type;*/

// TODO:
// Insert-methods with arguments that give only the first container value type
//  and the rest are filled with default values (enabled only if the rest container
//  value types are default constructible).
// Or even more generalized, given a certain container value type or a tuple with
//  a subset of the value types, insert the values correspondingly matching each container
//  and the rest are default assigned

// NOTE: All containers should follow the same update of keys logic
// for example:
// - All are std::vector
// - All UnorderedVector<std::vector>
// - All are std::list
// - First is std::list and others are a map

template <typename ...Containers>
class PropertyContainer {

  static_assert(sizeof...(Containers) != 0);
  static_assert((is_container_v<Containers> && ...));
  static_assert(!is_map_container_v<first_type_t<Containers...>>);

  // TODO: When the first container is not random access, then the rest of the containers, should be map like container 
  /*static_assert(
    !!std::is_base_of_v<
      typename std::random_access_iterator_tag,
      typename std::iterator_traits<
        typename first_type_t<Containers...>::iterator>::iterator_category
      >
    // || (is_map_container_v<Containers> && ...));
    // NOTE: fold expression with an initializer to skip the first type:
    || ((false || is_map_container_v<Containers>) && ...));*/

  // static_assert(!is_key_handle_v<KeyTag>);
  // static_assert(std::is_empty_v<KeyTag>);

  using ThisType = PropertyContainer<Containers...>;
  using TupType = std::tuple<Containers...>;
  using FirstContainer = first_type_t<Containers...>;

  template <typename Cont>
  using Traits = key_cont_traits<Cont>;

  TupType _conts;

protected:

  /*template <size_t Index>
  using id_to_cont_type = std::tuple_element_t<Index, TupType>;
  
  template <typename ValueType>
  using value_to_cont_type = find_type_by_t<
    // get_value_type,
    __key_cont_to_value,
    ValueType,
    Containers...
  >;
  
  template <typename ValueType>
  static constexpr size_t value_to_id = find_index_by_v<
    // get_value_type,
    __key_cont_to_value,
    ValueType,
    Containers...
  >;
  
  template <typename Val>
  static constexpr bool contains_value = tuple_contains_type_by_v<__key_cont_to_value, Val, TupType>;
  
  template <size_t ID>
  static constexpr bool contains_id = ID < std::tuple_size_v<TupType>;

  template <typename ValueType>
  value_to_cont_type<ValueType>& get() {
    static_assert(!std::is_same_v<void, value_to_cont_type<ValueType>>);
    return std::get<value_to_id<ValueType>>(_conts);
  }

  template <typename ValueType>
  const value_to_cont_type<ValueType>& get() const {
    static_assert(!std::is_same_v<void, value_to_cont_type<ValueType>>);
    return std::get<value_to_id<ValueType>>(_conts);
  }
  
  template <size_t Index>
  id_to_cont_type<Index>& get() {
    return std::get<Index>(_conts);
  }
  
  template <size_t Index>
  const id_to_cont_type<Index>& get() const {
    return std::get<Index>(_conts);
  }*/

public:

  template <size_t ID>
  using sub_container = std::tuple_element_t<ID, TupType>;
  
  template <size_t ID>
  using sub_container_view = View<typename sub_container<ID>::iterator>;
  
  template <size_t ID>
  using sub_container_cview = View<typename sub_container<ID>::const_iterator>;

  template <size_t ID>
  const sub_container<ID>& sub() const {
    return std::get<ID>(_conts);
  }
  
  template <size_t ID>
  sub_container<ID>& sub() {
    return std::get<ID>(_conts);
  }

  // NOTE: For the non-const version we need to give a view, so
  // the user can change the values, but not resize the container
  // template <size_t ID>
  // sub_container_view<ID> sub() {
  //   return View(std::get<ID>(_conts).begin(), std::get<ID>(_conts).end());
  // }
  
 
  using difference_type = typename Traits<FirstContainer>::difference_type;
  using size_type = typename Traits<FirstContainer>::size_type;

  using key_type = typename key_cont_traits<FirstContainer>::key_type;
  using key_iterator = typename key_cont_traits<FirstContainer>::key_iterator;

  using value_type = typename std::tuple<typename key_cont_traits<Containers>::value_type...>;
  using reference = typename std::tuple<typename key_cont_traits<Containers>::reference...>;
  // using pointer = typename std::tuple<typename key_cont_traits<Containers>::reference...>;
  using const_reference = typename std::tuple<typename key_cont_traits<Containers>::const_reference...>;

  using rvalue_reference = typename std::tuple<typename key_cont_traits<Containers>::value_type&&...>;
  // using rvalue_reference = typename std::tuple<std::remove_reference_t<typename key_cont_traits<Containers>::reference>&&...>;

  // using iterator = zip_iterator<typename key_cont_traits<Containers>::iterator...>;
  // using const_iterator = zip_iterator<typename key_cont_traits<Containers>::const_iterator...>;
  
  using iterator = LookupIterator<key_iterator, ThisType>;
  using const_iterator = LookupIterator<key_iterator, const ThisType>;
  
  // TODO:
  // using iterator = ZipIterator<typename key_cont_traits<Containers>::value_iterator...>;
  // using const_iterator = ZipIterator<typename key_cont_traits<Containers>::value_const_iterator...>;

  // BUG: I dont think this is correct, but lets keep it, so other container wrappers dont scream
  using allocator_type = std::tuple<typename Containers::allocator_type...>;
  

  PropertyContainer() {}

  PropertyContainer(const Containers&... conts)
  : _conts(conts...) {
    size_type n = std::get<0>(_conts);

    bool is_correct = std::apply([](const Containers&... cs) {
      return ((cs.size() == n) && ... && true);
    }, _conts);

    if (!is_correct)
      throw std::invalid_argument("All containers used to construct a PropertyContainer should have the same size");
  }
  
  PropertyContainer(Containers&&... conts)
  : _conts(std::move(conts)...) {
    size_type n = std::get<0>(_conts).size();

    bool is_correct = std::apply([n](const Containers&... cs) {
      return ((cs.size() == n) && ... && true);
    }, _conts);

    if (!is_correct)
      throw std::invalid_argument("All containers used to construct a PropertyContainer should have the same size");
  }

  PropertyContainer(size_type n, const value_type& val) {
    insert(end(), n, val);
  }
  
  template <typename ValIter, typename = std::enable_if_t<is_iterator_v<ValIter>>>
  PropertyContainer(ValIter first, ValIter last) {
  // PropertyContainer(const ValIter& first, const ValIter& last) {
    static_assert(is_std_tuple_v<typename std::iterator_traits<ValIter>::value_type>);
    insert(end(), first, last);
  }

  PropertyContainer(std::initializer_list<value_type> l)
  : PropertyContainer(l.begin(), l.end()) {}


  /*iterator begin() {
    return std::apply([](Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vbegin(conts)...);
    }, _conts);
  }
 
  iterator end() {
    return std::apply([](Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vend(conts)...);
    }, _conts);
  }
  
  const_iterator begin() const {
    return std::apply([](const Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vbegin(conts)...);
    }, _conts);
  }
 
  const_iterator end() const {
    return std::apply([](const Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vend(conts)...);
    }, _conts);
  }
  
  const_iterator cbegin() const {
    return std::apply([](const Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vbegin(conts)...);
    }, _conts);
  }
 
  const_iterator cend() const {
    return std::apply([](const Containers&... conts){
      return zip_iterator(key_cont_traits<Containers>::vend(conts)...);
    }, _conts);
  }*/

  // NOTE:
  // The first container with its key_cont_traits, dictates the key iteration
  //  and based on this key iteration, all other containers follow up this order
  //  so if the other property containers are unordered, it should still be fine
  // (Thats why we dont use the zip_iterator, otherwise the
  //  ordering per each property container, will be a mess)
  
  key_iterator key_begin() const { return key_cont_traits<FirstContainer>::begin(std::get<0>(_conts)); }
  key_iterator key_end() const { return key_cont_traits<FirstContainer>::end(std::get<0>(_conts)); }
  // Iterable<key_iterator> keys() const { return Iterable(key_begin(), key_end()); }

  iterator begin() {
    return iterator(key_begin(), *this);
    // return make_lookup_iterator(key_begin(), *this);
  }

  iterator end() {
    return iterator(key_end(), *this);
    // return make_lookup_iterator(key_end(), *this);
  }

  const_iterator begin() const {
    return const_iterator(key_begin(), *this);
    // return make_lookup_iterator(key_begin(), *this);
  }

  const_iterator end() const {
    return const_iterator(key_end(), *this);
    // return make_lookup_iterator(key_end(), *this);
  }

  const_iterator cbegin() const {
    return const_iterator(key_begin(), *this);
    // return make_lookup_iterator(key_begin(), *this);
  }

  const_iterator cend() const {
    return const_iterator(key_end(), *this);
    // return make_lookup_iterator(key_end(), *this);
  }


  bool empty() const {
    return std::get<0>(_conts).empty();
  }

  size_type size() const {
    return std::get<0>(_conts).size();
  }

  void clear() {
    std::apply([](Containers&... attribute) { (attribute.clear(), ...); }, _conts);
  }

  void resize(size_type n) {
    std::apply([n](Containers&... attribute) { (attribute.resize(n), ...); }, _conts);
  }

  void reserve(size_type n) {
    /*std::apply([n](auto&... attribute) {
      if constexpr (std::is_void_v<std::void_t<decltype(attribute.reserve(n))>>)
      (attribute.resize(n), ...);

      // (void)std::initializer_list<int>{
      //   (if constexpr (std::is_integral_v<std::decay_t<Args>>) {
      //       std::cout << "Integer: " << args << '\n';
      //   } else {
      //       std::cout << "Non-integer: " << args << '\n';
      //   }, 0)...}; // Fold expression to expand the pack

    }, _key_conts);*/
    for_each([n](auto& cont){
      using cont_type = decltype(cont);
      if constexpr (std::is_void_v<std::void_t<decltype(std::declval<cont_type&>().reserve(n))>>)
      // if constexpr (requires(Containers& cont, size_type n){cont.reserve(n);})
        cont.reserve(n);
    });
  }

  // template <typename Cont = FirstContainer>
  // const base_type<Cont>& base() const {
  //   return std::get<Cont>(_key_conts);
  // }

  // key_iterator find(key_type key) const {
  //   return key_cont_traits<sub_container<0>>::find(key);
  // }
  
  iterator find(key_iterator key_it) {
    return iterator(key_it, *this);
  }
  
  const_iterator find(key_iterator key_it) const {
    return const_iterator(key_it, *this);
  }

  iterator find(key_type key) {
    return iterator(key_cont_traits<sub_container<0>>::find(key), *this);
  }

  const_iterator find(key_type key) const {
    return const_iterator(key_cont_traits<sub_container<0>>::find(key), *this);
  }

  bool contains(key_type key) const {
    // return get<0>().contains(key);
    // return key_cont_traits<id_to_cont_type<0>>::contains(key);
    return key_cont_traits<sub_container<0>>::contains(key);
  }

  void push_back(const value_type& value) {
    std::apply([&value](Containers&... cs){
      std::apply([&cs...](const typename key_cont_traits<Containers>::value_type&... vals){
        // (cs.push_back(vals), ...);
        (key_cont_traits<Containers>::insert(
          cs,
          key_cont_traits<Containers>::end(cs),
          vals
        ), ...);
      }, value);
    }, _conts);
  }

  // void push_back(value_type&& value) {
  //   std::apply([&value](Containers&... cs){
  //     std::apply([&cs...](typename key_cont_traits<Containers>::value_type&&... vals){
  //       (cs.push_back(std::move(vals)), ...);
  //     }, value);
  //   }, _conts);
  // }

  void push_back(rvalue_reference value) {
    std::apply([&value](Containers&... cs){
      std::apply([&cs...](typename key_cont_traits<Containers>::value_type&&... vals){
        // (cs.push_back(std::move(vals)), ...);
        (key_cont_traits<Containers>::insert(
          cs,
          key_cont_traits<Containers>::end(cs),
          std::move(vals)
        ), ...);
      }, value);
    }, _conts);
  }

  // NOTE: This should be enabled only when iterator category
  //  is at least bidirectional iterator tag
  void pop_back() {
    std::apply([](Containers&... cs){
      (key_cont_traits<Containers>::erase(
        cs,
        std::prev(key_cont_traits<Containers>::end(cs))
      ), ...);
    }, _conts);
  }

  iterator insert(const_iterator where, const value_type& value) {
    // key_iterator key_it = where.key_it();
    key_iterator key_it = where.base();

    std::apply([&](FirstContainer& cont0, auto&... cont1toN) {
      std::apply([&](const auto& val0, const auto&... val1toN) {
        key_it = key_iterator(Traits<FirstContainer>::insert(cont0, key_it, val0));
        (Traits<std::remove_reference_t<decltype(cont1toN)>>::insert(cont1toN, key_it, val1toN), ...);
      }, value);
    }, _conts);

    return iterator(key_it, *this);
  }

  iterator insert(const_iterator where, size_type n, const value_type& value) {
    // key_iterator key_it = where.key_it();
    key_iterator key_it = where.base();

    std::apply([&](FirstContainer& cont0, auto&... cont1toN) {
      std::apply([&](const auto& val0, const auto&... val1toN) {
        key_it = key_iterator(Traits<FirstContainer>::insert(cont0, key_it, n, val0));
        (Traits<std::remove_reference_t<decltype(cont1toN)>>::insert(cont1toN, key_it, n, val1toN), ...);
      }, value);
    }, _conts);

    return iterator(key_it, *this);
  }
  
  // NOTE: No need for this overload since unzip_iterator<zip_iterator, N> specializes this case with the generalized one
  /*template <typename ...IterTypes>
  iterator insert(
    const_iterator where,
    zip_iterator<IterTypes...> first,
    zip_iterator<IterTypes...> last
  ) {
    key_iterator key_it = where.key_it();
    [&]<size_t I0, size_t... I1toN>(TupType& tup, const_iterator it, zip_iterator<IterTypes...> fir, zip_iterator<IterTypes...> las, std::index_sequence<I0, I1toN...>) {
      key_it = Traits<FirstContainer>::insert(std::get<I0>(tup), key_it, std::get<I0>(first.tuple()), std::get<I0>(last.tuple()));
      (Traits<std::tuple_element_t<I1toN, TupType>>::insert(std::get<I1toN>(tup), key_it, std::get<I1toN>(first.tuple()), std::get<I1toN>(last.tuple())), ...);
    }(_conts, where, first, last, std::make_index_sequence<std::tuple_size_v<TupType>>{});
    return iterator(*this, key_it);
  }*/
  
  template <typename ValIter, typename = std::enable_if_t<is_iterator_v<ValIter>>>
  iterator insert(
    const_iterator where,
    ValIter first,
    ValIter last
  ) {
    using ValOfIter = typename std::iterator_traits<ValIter>::value_type;
    static_assert(is_std_tuple_v<ValOfIter>);
    static_assert(std::tuple_size_v<value_type> == std::tuple_size_v<ValOfIter>);

    // key_iterator key_it = where.key_it();
    key_iterator key_it = where.base();

    [&]<size_t I0, size_t... I1toN>(TupType& tup, const_iterator it, ValIter fir, ValIter las, std::index_sequence<I0, I1toN...>) {
      // key_it = Traits<FirstContainer>::insert(std::get<I0>(tup), key_it, TransformIterator(first, TupleGetFunc<I0>()), TransformIterator(last, TupleGetFunc<I0>()));
      // (Traits<std::tuple_element_t<I1toN, TupType>>::insert(std::get<I1toN>(tup), key_it, TransformIterator(first, TupleGetFunc<I1toN>()), TransformIterator(last, TupleGetFunc<I1toN>())), ...);
      key_it = Traits<FirstContainer>::insert(std::get<I0>(tup), key_it, make_unzip_iterator<I0>(first), make_unzip_iterator<I0>(last));
      (Traits<std::tuple_element_t<I1toN, TupType>>::insert(std::get<I1toN>(tup), key_it, make_unzip_iterator<I1toN>(first), make_unzip_iterator<I1toN>(last)), ...);
    }(_conts, where, first, last, std::make_index_sequence<std::tuple_size_v<TupType>>{});

    return iterator(key_it, *this);
  }


  iterator insert(const_iterator where, std::initializer_list<value_type> l) {
    return insert(where, l.begin(), l.end());
  }


  iterator erase(const_iterator where) {
    // key_iterator key_it = where.key_it();
    key_iterator key_it = where.base();

    std::apply([&](FirstContainer& cont0, auto&... cont1toN) {
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::erase(cont1toN, key_it), ...);
      key_it = key_iterator(Traits<FirstContainer>::erase(cont0, key_it));
    }, _conts);

    return iterator(key_it, *this);
  }
  
  iterator erase(const_iterator first, const_iterator last) {
    key_iterator key_it = std::apply([&](FirstContainer& cont0, auto&... cont1toN) {
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::erase(cont1toN, first.base(), last.base()), ...);
      return key_iterator(Traits<FirstContainer>::erase(cont0, first.base(), last.base()));
    }, _conts);

    return iterator(key_it, *this);
  }


  reference at(key_type key) {
    return std::apply([key](Containers&... conts){
      return std::forward_as_tuple(key_cont_traits<Containers>::at(conts, key)...);
    }, _conts);
  }

  const_reference at(key_type key) const {
    return std::apply([key](const Containers&... conts){
      return std::forward_as_tuple(key_cont_traits<Containers>::at(conts, key)...);
    }, _conts);
  }

  reference operator[](key_type key) {
    return std::apply([key](Containers&... conts){
      return std::forward_as_tuple(key_cont_traits<Containers>::at(conts, key)...);
    }, _conts);
  }

  const_reference operator[](key_type key) const {
    return std::apply([key](const Containers&... conts){
      return std::forward_as_tuple(key_cont_traits<Containers>::at(conts, key)...);
    }, _conts);
  }

};


// --------------------------------------------------------------------------------- //


// TODO: Consider this memory optimization
// unzip iterator out of a lookup iterator of PropertyContainer
// will just result in a lookup iterator of the corresponding ID
// sub-container lookup (reducing the memory of the iterator,
// by storing only the necessary one)

/*template <size_t ID, typename ...Conts>
inline auto make_unzip_iterator(const typename PropertyContainer<Conts...>::iterator& it) {
  return make_lookup_iterator(it.base(), it.functor().lookup().template sub<ID>());
}

template <size_t ID, typename ...Conts>
inline auto make_unzip_iterator(const typename PropertyContainer<Conts...>::const_iterator& it) {
  return make_lookup_iterator(it.base(), it.functor().lookup().template sub<ID>());
}*/


// --------------------------------------------------------------------------------- //


template <typename ...Containers>
class key_cont_traits<PropertyContainer<Containers...>> {

  using container = PropertyContainer<Containers...>;

public:

  using value_type = typename container::value_type;
  using reference = typename container::reference;
  using const_reference = typename container::const_reference;

  using value_iterator = typename container::iterator;
  using value_const_iterator = typename container::const_iterator;

  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;
  
  using key_type = typename container::key_type;
  using key_iterator = typename container::key_iterator;

  
  static bool contains(const container& cont, key_type key) {
    return cont.contains(key);
  }

  static key_iterator find(const container& cont, key_type key) {
    return cont.find(key).base();
  }


  static key_iterator begin(const container& cont) {
    return cont.key_begin();
  }

  static key_iterator end(const container& cont) {
    return cont.key_end();
  }

  static View<key_iterator> keys(const container& cont) {
    return {
      cont.key_begin(),
      cont.key_end()
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
      cont.begin(),
      cont.end()
    };
  }

  static View<value_const_iterator> cvalues(const container& cont) {
    return {
      cont.begin(),
      cont.end()
    };
  }


  static reference at(container& cont, key_type key) {
    return cont[key];
  }

  static const_reference at(const container& cont, key_type key) {
    return cont[key];
  }

  // =============================== KEY BASED RESIZE METHODS ================================== //

  static key_iterator insert(container& cont, key_iterator key_it, const value_type& val = value_type()) {
    return cont.insert(cont.find(key_it), val).base();
  }

  static key_iterator insert(container& cont, key_iterator key_it, size_type n, const value_type& val = value_type()) {
    return cont.insert(cont.find(key_it), n, val).base();
  }

  template <typename IterType, typename = std::enable_if_t<is_iterator_v<IterType>>>
  static key_iterator insert(container& cont, key_iterator key_it, IterType first, IterType last) {
    static_assert(std::is_convertible_v<
      typename std::iterator_traits<IterType>::value_type,
      value_type>
    );
    return cont.insert(cont.find(key_it), first, last).base();
  }

  static key_iterator erase(container& cont, key_iterator key_it) {
    return cont.erase(cont.find(key_it)).base();
  }

  static key_iterator erase(container& cont, key_iterator from, key_iterator to) {
    return cont.erase(cont.find(from), cont.find(to)).base();
  }

};


// template <typename ...Containers>
// PropertyContainer(const Containers&...)
// -> PropertyContainer<Containers...>;
//
// template <typename ...Containers>
// PropertyContainer(Containers&&...)
// -> PropertyContainer<std::remove_cvref_t<Containers>...>;



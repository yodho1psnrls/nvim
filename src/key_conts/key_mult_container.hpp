#pragma once

// #include "../utilities.hpp"
#include "../meta_func.hpp"
// #include "policies.hpp"
#include "../keys/key_traits.hpp"
#include "../meta_func.hpp"
// #include "keys.hpp"
#include "../keys/keys.hpp"

#include <tuple>
#include <type_traits>
#include <iterator>
#include <unordered_map>


// =========================================================== //


// If we define the first container of KeyMultContainer
//  to be a vector, it will choose vector for the next
//  type of containers, if not, then it will choose a map
template <template <typename> class FirstContainer, typename ValueType>
struct get_rest_cont_type {
  static_assert(is_container_v<FirstContainer<ValueType>>);
  // static_assert(is_key<FirstContainer>);
  using type = _if_t<
    std::is_base_of_v<
      std::random_access_iterator_tag,
      typename std::iterator_traits<
        typename FirstContainer<ValueType>::iterator
      >::iterator_category
    >,
    FirstContainer<ValueType>,
    std::unordered_map<
      typename key_cont_traits<FirstContainer<ValueType>>::key_type,
      ValueType
    >
  >;
};


/*template <
  typename Key,
  template<typename> class Container,
  typename ArrangePolicy,
  typename ...ValueTypes
>
class KeyMultContainer {

  template <typename ValType>
  using RestContType = get_rest_cont_type<Container, ValType>;

  using TupType = std::tuple<
    get_rest_cont_type<Container, ValueTypes>...
  >;


};*/

template <typename Container>
struct __key_cont_to_value {
  using type = typename key_cont_traits<Container>::value_type;
};

template <typename Container>
using __key_cont_to_value_t = typename __key_cont_to_value<Container>::type;


template <
  // typename KeyTag,
  // typename ArrangePolicy,
  typename ...Containers
>
class KeyMultContainer {

  static_assert(sizeof...(Containers) != 0);
  static_assert(!is_map_container_v<first_type_t<Containers...>>);

	// When the first container is not random access, then the rest of the containers, should be map like container 
  static_assert(
		!!std::is_base_of_v<
			typename std::random_access_iterator_tag,
			typename std::iterator_traits<
				typename first_type_t<Containers...>::iterator>::iterator_category
			>
		// || (is_map_container_v<Containers> && ...));
		// NOTE: fold expression with an initializer to skip the first type:
		|| ((false || is_map_container_v<Containers>) && ...));

  // static_assert(!is_key_handle_v<KeyTag>);
  // static_assert(std::is_empty_v<KeyTag>);

  using ThisType = KeyMultContainer<Containers...>;
  // using ThisType = KeyMultContainer<KeyTag, Containers...>;
  // using ThisType = KeyMultContainer<Key, ArrangePolicy, Containers...>;
  using TupType = std::tuple<Containers...>;
  using FirstContainer = first_type_t<Containers...>;
  // using FirstContainer = std::tuple_element_t<0, TupType>;

  template <typename Cont>
  using Traits = key_cont_traits<Cont>;

  TupType _conts;

  // friend pop_back_type_t<ThisType>;
  // template <typename NewCont>
  // friend push_back_type_t<ThisType, NewCont>;
  
  // template <typename...>
  // friend class KeyMultContainer;

public:
  
  // template <typename Cont = FirstContainer>
  // using base_type = typename Cont::base_type;

  template <size_t Index>
  using id_to_cont_type = std::tuple_element_t<Index, TupType>;
  
  // template <size_t Index>
  // using base_type = typename std::tuple_element_t<Index, TupType>::base_type;
  
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
  }

public:
  
  // TODO: Do move constructor from base_type
  
  /*template <typename ...OtherContainers>
  KeyMultContainer(OtherContainers&&... containers) {
    auto tup = std::forward_as_tuple(std::forward<OtherContainers>(containers)...);
    // using tup_type = decltype(tup);
    using tup_type = remove_cvref_from_tuple_t<decltype(tup)>;
    size_t size = 0;

    if constexpr (sizeof...(containers))
      // size = get_first_val(containers...).size();
      size = std::get<0>(tup).size();
    
    for_each([size, &tup](auto& att) {
      using att_type = std::remove_cvref_t<decltype(att)>;

      if constexpr (contains_type_v<tup_type, att_type>) {
        // NOTE: Permit different size containers, like the
        //  row ranges vector of the jagged matrix which size
        //  is one item bigger because it stores n ranges (of two iterators)
        //  which means that it stores n + 1 key positions
        // if (get_any<att_type>(tup).size() != size)
        //   throw std::out_of_range("Inconsistent key container size");

        // att = std::get<att_type>(tup);
        att = get_any<att_type>(tup);
      }
      else
        att.resize(size);
    }, _key_conts);
  }*/

  // TODO:
  // KeyMultContainer(const pop_back_type_t<ThisType>& smaller) {
  //   auto& last = std::get<std::tuple_size_v<TupType> - 1>(_key_conts);
  //   last.resize(smaller.size());
  //   _key_conts = tuple_cat(smaller._attrib, std::make_tuple(last));
  // }
  
  // TODO:
  // template <bool NON_EMPTY = sizeof...(Containers1toN)>
  // KeyMultContainer(const pop_back_type_t<ThisType>& smaller, const last_type_t<Containers1toN...>& cont);
  /*template <typename U = void, typename = std::enable_if_t<sizeof...(Containers1toN), U>>
  KeyMultContainer(const pop_back_type_t<ThisType>& smaller, const last_type_t<Containers1toN...>& cont) {
    _cont = smaller._cont;
    auto& last = std::get<std::tuple_size_v<decltype(_attrib)> - 1>(_attrib);
    if (cont.size() != smaller.size())
      throw std::out_of_range("Inconsistent key container size");
    last = cont;
    _attrib = tuple_cat(smaller._attrib, std::make_tuple(last));
  }*/
  
  // template <typename T>
  // KeyMultContainer(const push_back_type_t<ThisType, T>& bigger) {
  //
  // }

  // using value_type = std::tuple<typename Containers::value_type...>;
  /*template <typename Cont = FirstContainer>
  using value_type = typename Traits<Cont>::value_tyle;
  template <typename Cont = FirstContainer>
  using reference = typename Traits<Cont>::reference;
  template <typename Cont = FirstContainer>
  using const_reference = typename Traits<Cont>::const_reference;*/

  KeyMultContainer() {}

  KeyMultContainer(const Containers&... conts)
  : _conts(conts...) {}
  
  KeyMultContainer(Containers&&... conts)
  : _conts(std::move(conts)...) {}
  
  using difference_type = typename Traits<FirstContainer>::difference_type;
  using size_type = typename Traits<FirstContainer>::size_type;

	// using key_type = TaggedType<typename key_cont_traits<FirstContainer>::key_type, KeyTag>;
  // using key_iterator = TaggedIterator<typename key_cont_traits<FirstContainer>::key_iterator, KeyTag>;
  using key_type = typename key_cont_traits<FirstContainer>::key_type;
  using key_iterator = typename key_cont_traits<FirstContainer>::key_iterator;


  key_iterator begin() const {
    return key_iterator(Traits<FirstContainer>::begin(get<0>()));
  }
 
  key_iterator end() const {
    return key_iterator(Traits<FirstContainer>::end(get<0>()));
  }

	Iterable<key_iterator> keys() const {
		return Iterable(begin(), end());
	}
	
	// Iterable<value_iterator> values() const {
	// 	return Iterable(begin(), end());
	// }

  bool empty() const {
    return get<0>().empty();
  }
  
  size_type size() const {
    return get<0>().size();
  }
  
  void clear() {
    std::apply([](auto&... attribute) { (attribute.clear(), ...); }, _conts);
  }
  
  void resize(size_type n) {
    std::apply([n](auto&... attribute) { (attribute.resize(n), ...); }, _conts);
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
      if constexpr (std::is_void_v<std::void_t<
        decltype(std::declval<cont_type>().reserve(n))>>
      )
        cont.reserve(n);
    });
  }

  // template <typename Cont = FirstContainer>
  // const base_type<Cont>& base() const {
  //   return std::get<Cont>(_key_conts);
  // }
  
  key_iterator find(key_type key) const {
    // return get<0>().find(key);
    return key_cont_traits<id_to_cont_type<0>>::find(key);
  }

  bool contains(key_type key) const {
    // return get<0>().contains(key);
    return key_cont_traits<id_to_cont_type<0>>::contains(key);
  }


  key_iterator insert(key_iterator key_it) {
    std::apply([&key_it](FirstContainer& cont0, auto&... cont1toN) {
      key_it = key_iterator(Traits<FirstContainer>::insert(cont0, key_it));
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::insert(cont1toN, key_it), ...);
    }, _conts);
    return key_it;
  }

  key_iterator insert(key_iterator key_it, size_type n) {
    std::apply([&key_it, n](FirstContainer& cont0, auto&... cont1toN) {
      key_it = key_iterator(Traits<FirstContainer>::insert(cont0, key_it, n));
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::insert(cont1toN, key_it, n), ...);
    }, _conts);
    return key_it;
  }

  key_iterator erase(key_iterator key_it) {
    std::apply([&key_it](FirstContainer& cont0, auto&... cont1toN) {
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::erase(cont1toN, key_it), ...);
      key_it = key_iterator(Traits<FirstContainer>::erase(cont0, key_it));
    }, _conts);
    return key_it;
  }
  
  key_iterator erase(key_iterator first, key_iterator last) {
    std::apply([&first, last](FirstContainer& cont0, auto&... cont1toN) {
      (Traits<std::remove_reference_t<decltype(cont1toN)>>::erase(cont1toN, first, last), ...);
      first = key_iterator(Traits<FirstContainer>::erase(cont0, first, last));
    }, _conts);
    return first;
  }

  template <typename ValueType>
  // template <typename ValueType, typename = std::enable_if_t<contains_value<ValueType>>>
  // typename Traits<value_to_cont_type<ValueType>>::reference
  decltype(auto) at(key_type key) {
    static_assert(contains_value<ValueType>);
    return Traits<value_to_cont_type<ValueType>>::at(get<ValueType>(), key);
  }

  template <typename ValueType>
  // template <typename ValueType, typename = std::enable_if_t<contains_value<ValueType>>>
  // typename Traits<value_to_cont_type<ValueType>>::const_reference
  decltype(auto) at(key_type key) const {
    static_assert(contains_value<ValueType>);
    return Traits<value_to_cont_type<ValueType>>::at(get<ValueType>(), key);
  }
 
  template <size_t ID = 0>
  typename Traits<id_to_cont_type<ID>>::reference
  at(key_type key) {
    static_assert(contains_id<ID>);
    return Traits<id_to_cont_type<ID>>::at(get<ID>(), key);
  }

  template <size_t ID = 0>
  typename Traits<id_to_cont_type<ID>>::const_reference
  at(key_type key) const {
    static_assert(contains_id<ID>);
    return Traits<id_to_cont_type<ID>>::at(get<ID>(), key);
  }

};



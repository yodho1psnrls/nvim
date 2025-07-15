#pragma once

#include "../meta_func.hpp"
// #include "../utilities.hpp"
#include "../iterators/unzip_iterator.hpp"
#include "../iterators/zip_iterator.hpp"

#include <type_traits>
#include <tuple>
#include <utility> // std::move // std::integer_sequence

// NOTE: References:
// https://www.fluentcpp.com/2018/12/18/the-soa-vector-part-1-optimizing-the-traversal-of-a-collection/
// https://www.fluentcpp.com/2018/12/21/an-soa-vector-with-an-stl-container-interface-in-cpp/


// template <typename ...Containers>
// class ZipContainer;
//
// template <typename ...Values>
// using ZipVector = ZipContainer<std::vector<Values>...>;


template <typename ...Containers>
class ZipContainer {

  using tup_cont_type = std::tuple<Containers...>;
  using FirstCont = first_type_t<Containers...>;
  // using FirstCont = std::tuple_element_t<0, tup_cont_type>;

  tup_cont_type _data;

public:

  using base_type = typename std::tuple<Containers...>;

  using size_type = typename FirstCont::size_type;
  using difference_type = typename FirstCont::difference_type;

  using allocator_type = std::tuple<typename Containers::allocator_type...>;
  using value_type = std::tuple<typename Containers::value_type...>;

  using reference = std::tuple<typename Containers::reference...>;
  using const_reference = std::tuple<typename Containers::const_reference...>;
  
  // using rvalue_reference = std::tuple<typename Containers::rvalue_reference...>;
  // using rvalue_reference = std::tuple<typename Containers::reference&&...>;
  // using rvalue_reference = std::tuple<std::remove_reference_t<typename Containers::reference>&&...>;
  using rvalue_reference = std::tuple<typename Containers::value_type&&...>;
 
  using pointer = std::tuple<typename Containers::pointer...>;
  using const_pointer = std::tuple<typename Containers::const_pointer...>;

  using iterator = ZipIterator<typename Containers::iterator...>;
  using const_iterator = ZipIterator<typename Containers::const_iterator...>;

  // using reverse_iterator = boost::zip_iterator<std::tuple<typename Containers::reverse_iterator...>>;
  // using const_reverse_iterator = boost::zip_iterator<std::tuple<typename Containers::const_reverse_iterator...>>;

  // ======================================================================= //
  
  template <size_t ID = 0>
  using id_to_base_type = typename std::tuple_element_t<ID, base_type>::type;

  template <size_t ID = 0>
  using id_to_size_type = typename std::tuple_element_t<ID, size_type>::type;
  
  template <size_t ID = 0>
  using id_to_difference_type = typename std::tuple_element_t<ID, difference_type>::type;

  template <size_t ID = 0>
  using id_to_allocator_type = typename std::tuple_element_t<ID, allocator_type>::type;

  template <size_t ID = 0>
  using id_to_value_type = typename std::tuple_element_t<ID, value_type>::type;

  template <size_t ID = 0>
  using id_to_reference = typename std::tuple_element_t<ID, reference>::type;
  
  template <size_t ID = 0>
  using id_to_const_reference = typename std::tuple_element_t<ID, const_reference>::type;
 
  template <size_t ID = 0>
  using id_to_pointer = typename std::tuple_element_t<ID, pointer>::type;
  
  template <size_t ID = 0>
  using id_to_const_pointer = typename std::tuple_element_t<ID, const_pointer>::type;

  template <size_t ID = 0>
  using id_to_iterator = typename std::tuple_element_t<ID, iterator>::type;
  
  template <size_t ID = 0>
  using id_to_const_iterator = typename std::tuple_element_t<ID, const_iterator>::type;


private:

  template <std::size_t... I>
  ZipContainer(size_type n, const value_type& val, std::index_sequence<I...>)
      : _data(Containers(n, std::get<I>(val))...) {}

public:

  bool is_valid() const {
    return std::apply([this](const Containers&... conts){
      return ((conts.size() == size()) && ...);
    });
  }
  
  ZipContainer()
  : _data() {}

  template<typename Cont = first_type_t<Containers...>,
    typename = std::enable_if_t<(std::is_same_v<Cont, Containers> || ...)>>
  ZipContainer(const Cont& cont)
  : _data() {
    std::get<Cont>(_data) = cont;
    std::apply([&](Containers&... cont){
      (cont.resize(cont.size()), ...);
    }, _data);
  }
 
  ZipContainer(const Containers&... containers)
  : _data(containers...) {}

  ZipContainer(Containers&&... containers)
  : _data(std::move(containers)...) {}

  ZipContainer(size_type n, const value_type& val = value_type())
  : ZipContainer(n, val, std::make_index_sequence<sizeof...(Containers)>()) {}

  template <typename ValIter, typename = std::enable_if_t<is_iterator_v<ValIter>>>
  ZipContainer(const ValIter& first, const ValIter& last) {
    static_assert(is_std_tuple_v<typename std::iterator_traits<ValIter>::value_type>);
    insert(end(), first, last);
  }

  ZipContainer(std::initializer_list<value_type> l)
  : ZipContainer(l.begin(), l.end()) {}


  // MultContainer(const MultContainer& other) = default;
  // MultContainer(MultContainer&& other) = default;

  const base_type& base() const {
    return _data;
  }
  
  template <size_t N>
  const id_to_base_type<N>& base() const {
    return std::get<N>(_data);
  }

  bool empty() const {
    return std::get<0>(_data).empty();
  }
  
  size_type size() const {
    return std::get<0>(_data).size();
  }

  size_type capacity() const {
    return std::get<0>(_data).capacity();
  }

  // void resize(size_type n) {
  //   (std::get<Containers>(_data).resize(n), ...);
  // }

  void resize(size_type n) {
    // []<std::size_t... Is>(auto& data, size_type size, std::index_sequence<Is...>) {
    //   ((std::get<Is>(data).resize(size)), ...);
    // }(_data, n, std::index_sequence_for<Containers...>{});
    for_each(_data, [n](auto& cont){ cont.resize(n); });
  }

  // TODO: SFINAE it if there is at least one container
  // supporting reserve and for the implementation
  // make it to call reserve only for types that support it 
  // void reserve(size_type n) {
  //   (std::get<Containers>(_data).reserve(n), ...);
  // }
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
    /*for_each([n](auto& cont){
      using cont_type = decltype(cont);
      if constexpr (std::is_void_v<std::void_t<
        decltype(std::declval<cont_type>().reserve(n))>>
      )
        cont.reserve(n);
    });*/
    // NOTE: Parameter pack expansion with conditional
    std::apply([n](Containers&... conts){
      (..., ( 
        [&]() {
          // if constexpr (is_vector_container_v<std::decay_t<Containers>>)
          // if constexpr (std::is_void_v<std::void_t<decltype(std::declval<Containers&>().reserve(n))>>)
          if constexpr (requires(Containers& cont, size_type n){cont.reserve(n);})
            conts.reserve(n);
        }()
      ));
    }, _data);
  }
  
  reference front() {
    return std::apply([](Containers&... cont) {
      return std::forward_as_tuple(cont.front()...);
    }, _data);
  }
  
  const_reference front() const {
    return std::apply([](const Containers&... cont) {
      return std::forward_as_tuple(cont.front()...);
    }, _data);
  }
  
  reference back() {
    return std::apply([](Containers&... cont) {
      return std::forward_as_tuple(cont.back()...);
    }, _data);
  }
  
  const_reference back() const {
    return std::apply([](const Containers&... cont) {
      return std::forward_as_tuple(cont.back()...);
    }, _data);
  }

  template <typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference operator[](size_type i) {
    // return std::tuple{ std::get<Containers>(_data)[i]... };
    // return transform(_data, [&](auto& cont) { return cont[i]; });
    return std::apply([i](Containers&... cont) {
      return std::forward_as_tuple(cont[i]...);
    }, _data);
  }
  
  template <typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  const_reference operator[](size_type i) const {
    return std::apply([i](const Containers&... cont) {
      return std::forward_as_tuple(cont[i]...);
    }, _data);
  }

  template <typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  reference at(size_type i) {
    return std::apply([i](Containers&... cont) {
      return std::forward_as_tuple(cont.at(i)...);
    }, _data);
  }
  
  template <typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  const_reference at(size_type i) const {
    // return std::tuple{ std::get<Containers>(_data).at(i)... };
    return std::apply([i](const Containers&... cont) {
      return std::forward_as_tuple(cont.at(i)...);
    }, _data);
  }

  
  // template <size_t ID>
  // id_to_reference<ID> operator[](size_type i) {
  //   return std::get<ID>(_data)[i];
  // }
  // 
  // template <size_t ID>
  // id_to_const_reference<ID> operator[](size_type i) const {
  //   return std::get<ID>(_data)[i];
  // }

  template <size_t ID, typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  id_to_reference<ID> at(size_type i) {
    return std::get<ID>(_data).at(i);
  }
  
  template <size_t ID, typename IC = typename std::iterator_traits<iterator>::iterator_category,
    typename = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, IC>>>
  id_to_const_reference<ID> at(size_type i) const {
    return std::get<ID>(_data).at(i);
  }


  // ------------------------------------------------------------ //


  iterator begin() {
    return std::apply([](Containers&... conts){
      return iterator(conts.begin()...);
    }, _data);
  }

  iterator end() {
    return std::apply([](Containers&... conts){
      return iterator(conts.end()...);
    }, _data);
  }

  const_iterator begin() const {
    return std::apply([](const Containers&... conts){
      return const_iterator(conts.begin()...);
    }, _data);
  }
  
  const_iterator end() const {
    return std::apply([](const Containers&... conts){
      return const_iterator(conts.end()...);
    }, _data);
  }
  
  const_iterator cbegin() const {
    return std::apply([](const Containers&... conts){
      return const_iterator(conts.cbegin()...);
    }, _data);
  }
  
  const_iterator cend() const {
    return std::apply([](const Containers&... conts){
      return const_iterator(conts.cend()...);
    }, _data);
  }

  
  template <size_t ID>
  id_to_iterator<ID> begin() {
    return std::get<ID>(_data).begin();
  }

  template <size_t ID>
  id_to_iterator<ID> end() {
    return std::get<ID>(_data).end();
  }

  template <size_t ID>
  id_to_const_iterator<ID> begin() const {
    return std::get<ID>(_data).begin();
  }

  template <size_t ID>
  id_to_const_iterator<ID> end() const {
    return std::get<ID>(_data).end();
  }
  
  template <size_t ID>
  id_to_const_iterator<ID> cbegin() const {
    return std::get<ID>(_data).cbegin();
  }

  template <size_t ID>
  id_to_const_iterator<ID> cend() const {
    return std::get<ID>(_data).cend();
  }


  // ------------------------------------------------------------ //


  void push_back(const value_type& val) {
    std::apply([&val](Containers&... conts) {
      std::apply([&conts...](const typename Containers::value_type&... vals) {
        (conts.push_back(vals), ...);
      }, val);
    }, _data);
  }
  
  void push_back(rvalue_reference val) {
    std::apply([&val](Containers&... conts) {
      std::apply([&conts...](typename Containers::value_type&&... vals) {
        (conts.push_back(std::move(vals)), ...);
      }, val);
    }, _data);
  }

  void pop_back() {
    std::apply([](Containers&... conts) {
      (conts.pop_back(), ...);
    }, _data);
  }


  /*iterator insert(const const_iterator& where, const_reference val) {
    boost::tuple<typename Containers::iterator...> new_iters;

    ((boost::get<typename Containers::iterator>(new_iters) =
      boost::get<Containers>(_data).insert(
        boost::get<Containers>(where.get_iterator_tuple()),
        boost::get<typename Containers::const_reference>(val)
      )
    ),
    ...);

    return boost::make_zip_iterator(new_iters);
  }
  
  iterator insert(const const_iterator& where, rval_reference val) {
    boost::tuple<typename Containers::iterator...> new_iters;

    ((boost::get<typename Containers::iterator>(new_iters) =
      boost::get<Containers>(_data).insert(
        boost::get<Containers>(where.get_iterator_tuple()),
        std::move(boost::get<typename Containers::reference&>(val))
      )
    ),
    ...);

    return boost::make_zip_iterator(new_iters);
  }

  template <typename ...IterTypes>
  iterator insert(
    const const_iterator& where,
    const boost::zip_iterator<boost::tuple<IterTypes...>>& first,
    const boost::zip_iterator<boost::tuple<IterTypes...>>& last
  ) {
    boost::tuple<typename Containers::iterator...> new_iters;

    ((boost::get<typename Containers::iterator>(new_iters) =
      boost::get<Containers>(_data).insert(
        boost::get<Containers>(where.get_iterator_tuple()),
        boost::get<IterTypes>(first.get_iterator_tuple()),
        boost::get<IterTypes>(last.get_iterator_tuple())
      )
    ),
    ...);

    return boost::make_zip_iterator(new_iters);
  }*/

  // iterator insert(const_iterator where, const_reference val) {
  iterator insert(const_iterator where, const value_type& val) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, const_reference v, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), std::get<I>(v))...);
    }(_data, where, val, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
    // return std::apply([&](Containers&... conts){
    //  return std::apply([&](Containers::const_iterator... iters){
    //    return std::apply([&](const Containers::value_type&... vals){
    //      return iterator(conts.insert(iters, vals)...);
    //    }, val);
    //  }, where.tuple());
    // }, _data);
  }
  
  // iterator insert(const_iterator where, size_type count, const_reference val) {
  iterator insert(const_iterator where, size_type count, const value_type& val) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, size_type n, const_reference v, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), n, std::get<I>(v))...);
    }(_data, where, count, val, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }
  
  iterator insert(const_iterator where, rvalue_reference val) {
  // iterator insert(const_iterator where, value_type&& val) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, rvalue_reference v, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), std::move(std::get<I>(v)))...);
    }(_data, where, val, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }

  /*template <typename ...IterTypes>
  iterator insert(
    const_iterator where,
    ZipIterator<IterTypes...> first,
    ZipIterator<IterTypes...> last
  ) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, ZipIterator<IterTypes...> fir, ZipIterator<IterTypes...> las, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), std::get<I>(fir.tuple()), std::get<I>(las.tuple()))...);
    }(_data, where, first, last, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }*/
  
  template <typename IterType>
  iterator insert(
    const_iterator where,
    IterType first,
    IterType last
  ) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, IterType fir, IterType las, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), make_unzip_iterator<I>(fir), make_unzip_iterator<I>(las))...);
    }(_data, where, first, last, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }

  // BUG: This needs Fixing !
  iterator insert(
    const_iterator where,
    std::initializer_list<value_type> list
  ) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, const std::initializer_list<value_type>& l, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).insert(std::get<I>(it.tuple()), l)...);
    }(_data, where, list, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }


  iterator erase(const_iterator where) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator it, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).erase(std::get<I>(it.tuple()))...);
    }(_data, where, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }

  iterator erase(const_iterator first, const_iterator last) {
    return []<std::size_t... I>(tup_cont_type& t, const_iterator from, const_iterator to, std::index_sequence<I...>) {
      return make_zip_iterator(std::get<I>(t).erase(std::get<I>(from.tuple()), std::get<I>(to.tuple()))...);
    }(_data, first, last, std::make_index_sequence<std::tuple_size_v<tup_cont_type>>{});
  }


};



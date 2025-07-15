#pragma once

#include <utility> // std::declval
#include <type_traits>
#include <iterator>
#include <tuple>  // std::apply

// #include <tuple>
// #include <boost/tuple/tuple.hpp>

// https://en.cppreference.com/w/cpp/types/enable_if
// https://en.cppreference.com/w/cpp/types/is_base_of
// https://stackoverflow.com/questions/58189867/how-to-simplify-enable-if-alias-in-template-template-parameter
// https://en.cppreference.com/w/cpp/language/definition#ODR-use
// https://en.cppreference.com/w/cpp/utility/declval

// https://stackoverflow.com/questions/5423246/how-to-detect-if-a-type-is-an-iterator-or-const-iterator
//template<class IT, class T=decltype(*std::declval<IT>())>
//constexpr bool is_const_iterator() {   
//  return !std::is_assignable<decltype(*std::declval<IT>()), T>::value;
//}

// This seems to fail with tensor_iterator with const base iterator !
// But because it uses std::is_assignable, if we place SFINAE for the
// assignment operators of Iterable with const_iterator, then it may work correctly
template <typename IT>
struct is_const_iterator {
  using deref_type = decltype(*std::declval<IT>());
  static constexpr bool value = !std::is_assignable_v<deref_type, deref_type>;
};

template <typename IT>
constexpr bool is_const_iterator_v = is_const_iterator<IT>::value;


// https://www.youtube.com/watch?v=_yqIdYBdyPo&list=PLWxziGKTUvQFIsbbFcTZz7jOT4TMGnZBh&index=3
// https://youtu.be/_yqIdYBdyPo?si=eynVALVjhZvl45eB&t=462
// NOTE: Same as std::conditional
template <bool COND, typename THEN, typename ELSE = void>
struct _if;

template <typename THEN, typename ELSE>
struct _if<true, THEN, ELSE> {
  using type = THEN;
};

template <typename THEN, typename ELSE>
struct _if<false, THEN, ELSE> {
  using type = ELSE;
};

// NOTE: std::conditional_t equivalent
template <bool COND, typename THEN, typename ELSE = void>
using _if_t = typename _if<COND, THEN, ELSE>::type;



template <typename A, typename B, typename = typename
  std::enable_if_t<std::is_base_of_v<A, B> || std::is_base_of_v<B, A>>>
using base = _if<std::is_base_of_v<A, B>, A, B>;

template <typename A, typename B>
using base_t = typename base<A, B>::type;


template <typename ...Ts>
struct most_base;

template <typename T>
struct most_base<T> {
  using type = T;
};

template <typename First, typename Second, typename ...Rest>
struct most_base<First, Second, Rest...>
: public most_base<base_t<First, Second>, Rest...> {};

template <typename ...Ts>
using most_base_t = typename most_base<Ts...>::type;


template <typename A, typename B, typename = typename
std::enable_if_t<std::is_base_of_v<A, B> || std::is_base_of_v<B, A>>>
using derived = _if<std::is_base_of_v<A, B>, B, A>;

template <typename A, typename B>
using derived_t = typename derived<A, B>::type;



// Example:
/*struct BaseA {
  int a;
};

struct A : public BaseA {
  float b;

  A(int a, float b) : BaseA(a), b(b) {}
};*/

/*PartialConstructor<A, float> constructee(14.731f);
for(int i = 5; i < 12; ++i) {
  A val = constructee(i);
  std::cout << val.a << ", " << val.b << "\n";
}*/


// NOTE: Empty Class Optimization will only happen,
// if this class is inherited, and not stored as a value
/*template <typename T, typename ...LastArgs>
class PartialConstructor {
public:

  std::tuple<LastArgs...> largs;

  PartialConstructor(const LastArgs&... largs)
    : largs(largs...) {}

  //template <typename... FirstArgs, typename std::enable_if_t<std::is_constructible_v<T, FirstArgs..., LastArgs...>>>
  template <typename... FirstArgs>
  T operator()(FirstArgs&&... fargs) const {
    return std::apply(
      [&](const LastArgs&... args) {
          return T(std::forward<FirstArgs>(fargs)..., args...);
          //return T(std::forward<FirstArgs>(fargs)..., std::forward<LastArgs>(args)...);
      },
      largs  // Or use std::as_const(largs) if arguments are immutable
    );
  }

};*/



template <typename T, typename ...LastArgs>
class PartialConstructor {
/*protected:*/
public:

  std::tuple<LastArgs...> largs;
  //std::tuple<std::decay_t<LastArgs>...> largs; // Store arguments in a decayed form


  PartialConstructor(LastArgs&&... largs)
    /*: largs(std::forward(largs)...) {}*/
    : largs(largs...) {}

  //template <typename ...FirstArgs>
  //T operator()(FirstArgs&& ...fargs) const {
  //  return T(std::forward(fargs)..., std::forward(largs)...);
  //}

  template <typename... FirstArgs>
  //template <typename... FirstArgs, typename = std::enable_if_t<std::is_constructible_v<T, FirstArgs..., LastArgs...>>>
  //T operator()(FirstArgs&&... fargs) {
  constexpr T construct(FirstArgs&&... fargs) {
    return std::apply(
      /*[&](LastArgs&&... args) {*/
      [&](auto&&... args) {
        /*return T(std::forward<FirstArgs>(fargs)..., std::forward<LastArgs>(args)...);*/
        return T(std::forward<FirstArgs>(fargs)..., std::forward<decltype(args)>(args)...);
      },
      // std::move(largs)  // Or use std::as_const(largs) if arguments are immutable
      /*std::forward<LastArgs>(largs)...*/
      largs
    );
  }

};


template <typename T>
class PartialConstructor<T> {
/*protected:*/
public:

  template <typename... FirstArgs>
  /*template <typename... FirstArgs, typename = std::enable_if_t<std::is_constructible_v<T, FirstArgs...>>>*/
  static constexpr T construct(FirstArgs&&... fargs) {
    static_assert(std::is_constructible_v<T, FirstArgs...>);
    return T(std::forward<FirstArgs>(fargs)...);
  }

};


// -------------------------------------------------------------- //


// Metafunction to check if a type is an iterator
template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<
  T, 
  std::void_t<typename std::iterator_traits<T>::iterator_category>
> : std::true_type {};

// Helper variable template for is_iterator
template <typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;


// Metafunction to check if a type is a container
template <typename T, typename = void>
struct is_container
: std::false_type {};

// NOTE: Previous is_container
// template <typename T>
// struct is_container<
//   T,
//   std::void_t<
//       decltype(std::declval<T>().begin()), 
//       decltype(std::declval<T>().end())
//   >
// > : std::true_type {};

template <typename ContType>
class is_container<
  ContType,
  std::void_t<
    decltype(std::declval<ContType>().begin()), 
    decltype(std::declval<ContType>().end()),
    decltype(std::declval<ContType>().cbegin()), 
    decltype(std::declval<ContType>().cend()),
    typename ContType::iterator,
    typename ContType::const_iterator
  >
> {

  using T = std::remove_const_t<ContType>;

public:

  static constexpr bool value =
    std::is_same_v<
      decltype(std::declval<T>().begin()),
      typename T::iterator
    > &&
    std::is_same_v<
      decltype(std::declval<const T>().begin()),
      typename T::const_iterator
    > &&
    std::is_same_v<
      decltype(std::declval<const T>().cbegin()),
      typename T::const_iterator
    >;

};

// Is a Sequence Container (see https://en.cppreference.com/w/cpp/named_req/SequenceContainer)
template <typename T>
constexpr bool is_container_v = is_container<T>::value;


template <typename T, typename = void>
struct is_range
: public std::false_type {};

template <typename T>
struct is_range<
  T,
  std::void_t<
      decltype(std::declval<T>().begin()), 
      decltype(std::declval<T>().end())
      // typename T::iterator   // std::ranges::subrange doesnt have that
  >
> : public std::true_type {};

template <typename T>
constexpr bool is_range_v = is_range<T>::value;


template <typename T>
struct is_view {
  static constexpr bool value = is_range_v<T> && !is_container_v<T>;
};

/*template <typename T>
struct is_view<
  T,
  std::void_t<
    decltype(std::declval<T>().begin()), 
    decltype(std::declval<T>().end()),
    typename T::iterator
  >
> {
  // NOTE: Those are for is_view_v
  // static constexpr bool value = !is_container_v<T>;
  static constexpr bool value = 
    std::is_same_v<
      std::remove_cvref_t<decltype(std::declval<const T>().begin())>,
      typename T::iterator
    > && !is_container_v<T>;
};*/

template <typename T>
constexpr bool is_view_v = is_view<T>::value;



/*template <typename Container, typename = void>
struct is_key_container
: std::false_type {};

template<typename KeyContainer>
class is_key_container<
  KeyContainer,
  std::void_t<
    typename KeyContainer::key_type,
    typename KeyContainer::key_iterator,
    decltype(std::declval<KeyContainer>().begin())
  >
> {
  using KeyType = typename KeyContainer::key_type;
  using KeyIter = typename KeyContainer::key_iterator;
public:
  // static constexpr bool value = true;
  static constexpr bool value = 
    std::is_same_v<
      typename KeyContainer::key_iterator,
      decltype(std::declval<KeyContainer>().begin())
    > &&
    std::is_convertible_v<
      typename std::iterator_traits<KeyIter>::value_type,
      KeyType
    >;
};

template <typename Container>
constexpr bool is_key_container_v = is_key_container<Container>::value;*/


// template <typename T, typename = void>
// struct is_matrix : std::false_type {};
//
// template <typename T>
// struct is_matrix<
//   T,
//   std::void_t<
//     
//   >
// > : std::true_type {};
// Doesnt work with is_matrix_v<int>
// template <typename T>
// constexpr bool is_matrix_v =
//   is_container_v<T> &&
//   is_container_v<typename T::value_type>;

template <typename T, typename = void>
constexpr bool is_matrix_v = false;

template <typename T>
constexpr bool is_matrix_v<T, std::void_t<typename T::value_type>> =
    is_container_v<T> && is_container_v<typename T::value_type>;



template <typename T>
struct abs_base {
  using type = T;
};

template <template <typename> class Outer, typename Inner>
struct abs_base<Outer<Inner>> {
  using type = typename abs_base<Inner>::type;
};

template <typename T>
using abs_base_t = typename abs_base<T>::type;



template <typename T>
struct abs_base_first {
    using type = T;
};

// Specialization: Handles recursive templates with multiple arguments
template <template <typename, typename...> class Outer, typename Inner, typename... Rest>
struct abs_base_first<Outer<Inner, Rest...>> {
    using type = typename abs_base_first<Inner>::type;
};

// Helper alias for convenience
template <typename T>
using abs_base_first_t = typename abs_base_first<T>::type;




// Helper to compute the nesting depth of a type
template <typename T>
struct depth {
    static constexpr std::size_t value = 0;
};

template <template <typename> class Outer, typename Inner>
struct depth<Outer<Inner>> {
    static constexpr std::size_t value = 1 + depth<Inner>::value;
};

template <typename T1, typename T2>
struct diff_base {
    static constexpr std::size_t value = depth<T2>::value - depth<T1>::value;
};

template <typename T1, typename T2>
constexpr std::size_t diff_base_v = diff_base<T1, T2>::value;


// ================================================================== //


// Base case: Default to void when nothing matches
template <typename T, template <typename> class Target>
struct find_below {
    using type = void;
};

// Specialization: When `Target<...>` is exactly matched
template <template <typename> class Target, typename Inner>
struct find_below<Target<Inner>, Target> {
    using type = Inner;
};

// Recursive case: For a nested structure `Target<Inner>`
template <template <typename> class Target, typename Inner>
struct find_below<Target<Target<Inner>>, Target> {
    using type = typename find_below<Target<Inner>, Target>::type;
};

// Alias template for convenience
template <typename T, template <typename> class Target>
using find_below_t = typename find_below<T, Target>::type;

//static_assert(std::is_same_v<int, find_below_t<Foo<Foo<Foo<int>>>, Foo>>);
//static_assert(std::is_same_v<Baz<Baz<int>>, find_below_t<Foo<Foo<Foo<Baz<Baz<int>>>>>, Foo>>);




// Base case: if no match found, we return `void`
template <typename T, template <typename...> class Target>
struct find_below_first {
  using type = void;  // Default type if no match is found
};

// Recursive case: when we find the first non-template type, `Inner`, in Target<Inner, Args...>
template <template <typename...> class Target, typename Inner, typename... Args>
struct find_below_first<Target<Inner, Args...>, Target> {
  using type = Inner;  // Found the inner type
};

// Recursively handle nested types: target<Target<Inner, Args...>> and extract Inner
template <template <typename...> class Target, typename Inner, typename... Args1, typename... Args2>
struct find_below_first<Target<Target<Inner, Args1...>, Args2...>, Target> {
  using type = typename find_below_first<Target<Inner, Args1...>, Target>::type;
};

// Helper alias template for easier usage
template <typename T, template <typename...> class Target>
using find_below_first_t = typename find_below_first<T, Target>::type;

//using result = find_below_first_t<MultFoo<MultFoo<int, double>, float>, MultFoo>;
//static_assert(std::is_same_v<int, result>);


// ======================================================================= //


// This is just sizeof...(Parameter Pack)
/*template <typename ...Args>*/
/*struct num_of_args;*/
/**/
/*template <>*/
/*struct num_of_args<> {*/
/*  static constexpr std::size_t value = 0U;*/
/*};*/
/**/
/*template <typename First, typename ...Rest>*/
/*struct num_of_args<First, Rest...> {*/
/*  static constexpr std::size_t value = 1U + num_of_args<Rest...>::value;*/
/*};*/

//template <typename ...Args>
//constexpr num_of_args_v = num_of_args<Args...>::value;



template <int Index, typename T>
struct IndexedType : public T {

  using type = T;
  static constexpr int index = Index;

  using T::T;
  // IndexedType() : T() {}
  IndexedType(const T& val) : T(val) {}
  IndexedType(T&& val) : T(val) {}
 
  // operator T&() { return static_cast<T&>(*this); }
  // operator const T&() const { return static_cast<const T&>(*this); }
  // operator T*() { return static_cast<T*>(this); }
  // operator const T*() const { return static_cast<const T*>(this); }
};


// Same principle as IndexedType, but with a type as a tag,
//  instead of a index
// See: https://fanael.github.io/breaking-empty-base-optimization.html
/*template <typename Tag, typename T>
struct TaggedType : public T {

  using type = T;
  using tag = Tag;

  using T::T;
  TaggedType(const T& val) : T(val) {}
  TaggedType(T&& val) : T(val) {}
 
};*/


// This is a helper class for multiple inheritance, where
// each inherited type is tagged with a unique number
// so if there are duplicate types, we face no problem
// of ambiguity
template <typename ...Args>
//class TaggedMultipleInheritance {
class IndexedMultipleInheritance;

// Base case
template<>
class IndexedMultipleInheritance<> {};

// Recursive inheritance of all index tagged types
template <typename First, typename ...Rest>
struct IndexedMultipleInheritance<First, Rest...>
: public IndexedType<sizeof...(Rest), First>
, public IndexedMultipleInheritance<Rest...> {

  // template <typename T>
  // T& get() { return static_cast()}

};


// template <typename T>
// T& 


template <typename First, typename ...Rest>
struct first_type {
  using type = First;
};

template <typename ...Args>
using first_type_t = first_type<Args...>::type;


template <typename... Ts>
struct last_type;

template <typename T>
struct last_type<T> {
    using type = T;
};

template <typename First, typename... Rest>
struct last_type<First, Rest...> {
    using type = typename last_type<Rest...>::type;
};

template <typename... Ts>
using last_type_t = typename last_type<Ts...>::type;

template <size_t ID, typename ...Ts>
struct nth_type;

template <typename T0, typename ...T1toN>
struct nth_type<0, T0, T1toN...> {
  using type = T0;
};

template <size_t ID, typename T0, typename ...T1toN>
struct nth_type<ID, T0, T1toN...>
: public nth_type<ID - 1, T1toN...> {
  static_assert(ID < 1 + sizeof...(T1toN));
};

template <size_t ID, typename ...Ts>
using nth_type_t = typename nth_type<ID, Ts...>::type;


template <typename Tup>
struct tuple_first_type;

template <template <typename...> class Tup, typename ...Args>
struct tuple_first_type<Tup<Args...>>
: public first_type<Args...> {};

template <typename Tup>
using tuple_first_type_t = typename tuple_first_type<Tup>::type;

template <typename Tup>
struct tuple_last_type;

template <template <typename...> class Tup, typename ...Args>
struct tuple_last_type<Tup<Args...>>
: public last_type<Args...> {};

template <typename Tup>
using tuple_last_type_t = typename tuple_last_type<Tup>::type;


// https://stackoverflow.com/questions/16337610/how-to-know-if-a-type-is-a-specialization-of-stdvector

// =================================================================== //

template <typename ...Types>
struct type_list {};


template <typename T>
struct is_std_tuple
: std::false_type {};

template <typename ...Types>
struct is_std_tuple<std::tuple<Types...>>
: std::true_type {};

template <typename T>
constexpr bool is_std_tuple_v = is_std_tuple<std::remove_cvref_t<T>>::value;


// NOTE:
/*template <typename T>
struct is_boost_tuple
: std::false_type {};

template <typename ...Types>
struct is_boost_tuple<boost::tuple<Types...>>
: std::true_type {};

template <typename T>
constexpr bool is_boost_tuple_v = is_boost_tuple<std::remove_cvref_t<T>>::value;*/



template <template <typename, typename> class BinaryMetaFunc, typename... Types>
struct reduce;

// Similar to boost::hana::fold_left
template <template <typename, typename> class BinaryMetaFunc, typename T0, typename T1, typename ...T2toN>
struct reduce<BinaryMetaFunc, T0, T1, T2toN...> {
  using type = typename reduce<BinaryMetaFunc, typename BinaryMetaFunc<T0, T1>::type, T2toN...>::type;
};

template <template <typename, typename> class BinaryMetaFunc, typename T0, typename T1>
struct reduce<BinaryMetaFunc, T0, T1> {
  using type = typename BinaryMetaFunc<T0, T1>::type;
};

template <template <typename, typename> class BinaryMetaFunc, typename T0>
struct reduce<BinaryMetaFunc, T0> {
  using type = T0;
};

template <template <typename, typename> class BinaryMetaFunc, typename ...Types>
using reduce_t = typename reduce<BinaryMetaFunc, Types...>::type;


// ------------------------------------------------------------------------- //


template <typename Container>
struct get_value_type {
  static_assert(is_container_v<Container>);
  using type = typename Container::value_type;
};

template <typename T>
using get_value_type_t = typename get_value_type<T>::type;


template <typename Container>
struct get_size_type {
  static_assert(is_container_v<Container>);
  using type = typename Container::size_type;
};

template <typename T>
using get_size_type_t = typename get_size_type<T>::type;


template <typename Container>
struct get_iterator_type {
  static_assert(is_container_v<Container>);
  using type = typename Container::iterator;
};

template <typename T>
using get_iterator_type_t = typename get_iterator_type<T>::type;


template <typename Container>
struct get_const_iterator_type {
  static_assert(is_container_v<Container>);
  using type = typename Container::const_iterator;
};

template <typename T>
using get_const_iterator_type_t = typename get_const_iterator_type<T>::type;


template <template <typename> class Transform, typename Search, typename... Types>
struct find_index_by;

// Base case: Not found
template <template <typename> class Transform, typename Search>
struct find_index_by<Transform, Search> {
    static constexpr std::size_t value = static_cast<std::size_t>(-1); // Sentinel value
};

// Recursive case: Check first type, then continue
template <template <typename> class Transform, typename Search, typename First, typename... Rest>
struct find_index_by<Transform, Search, First, Rest...> {
  static constexpr std::size_t value =
    std::is_same_v<Search, typename Transform<First>::type> ? 0
    : (find_index_by<Transform, Search, Rest...>::value == static_cast<std::size_t>(-1) ? 
      static_cast<std::size_t>(-1) 
      : 1 + find_index_by<Transform, Search, Rest...>::value);
};

template <template <typename> class Transform, typename Search, typename... Types>
constexpr std::size_t find_index_by_v = find_index_by<Transform, Search, Types...>::value;

template <typename Search, typename ...Types>
struct find_index
: public find_index_by<std::type_identity, Search, Types...> {};

template <typename Search, typename ...Types>
constexpr std::size_t find_index_v = find_index<Search, Types...>::value;



template <template <typename> class Transform, typename Search, typename Tup>
struct tuple_find_index_by;

template <template <typename> class Transform, typename Search, template <typename...> class Tup, typename ...Args>
struct tuple_find_index_by<Transform, Search, Tup<Args...>>
: public find_index_by<Transform, Search, Args...> {};

template <template <typename> class Transform, typename Search, typename Tup>
constexpr std::size_t tuple_find_index_by_v = tuple_find_index_by<Transform, Search, Tup>::value;

template <typename Search, typename Tup>
struct tuple_find_index
: public tuple_find_index_by<std::type_identity, Search, Tup> {};


template <typename Search, typename Tup>
constexpr std::size_t tuple_find_index_v = tuple_find_index<Search, Tup>::value;


// ------------------------------------------------------------------------- //


template <template <typename> class Transform, typename Search, typename... Types>
struct find_type_by;

// Base case: Not found
template <template <typename> class Transform, typename Search>
struct find_type_by<Transform, Search> {
  using type = void;
};

// Recursive case: Check first type, then continue
template <template <typename> class Transform, typename Search, typename First, typename... Rest>
struct find_type_by<Transform, Search, First, Rest...> {
  using type = _if_t<
    std::is_same_v<Search, typename Transform<First>::type>,
    First,
    typename find_type_by<Transform, Search, Rest...>::type
  >;
};

template <template <typename> class Transform, typename Search, typename... Types>
using find_type_by_t = typename find_type_by<Transform, Search, Types...>::type;


template <template <typename> class Transform, typename Search, typename Tup>
struct tuple_find_type_by;

template <template <typename> class Transform, typename Search, template <typename...> class Tup, typename... Args>
struct tuple_find_type_by<Transform, Search, Tup<Args...>>
: public find_type_by<Transform, Search, Args...> {};

template <template <typename> class Transform, typename Search, typename Tup>
using tuple_find_type_by_t = typename tuple_find_type_by<Transform, Search, Tup>::type;


/*template <typename Search, typename ...Types>
struct contains_type;

template <typename Search>
struct contains_type<Search> : std::false_type {};

template <typename Search, typename ...Rest>
struct contains_type<Search, Search, Rest...> : std::true_type {};

template <typename Search, typename First, typename ...Rest>
struct contains_type<Search, First, Rest...> {
  static constexpr bool value = contains_type<Search, Rest...>::value;
};

template <typename Search, typename ...Types>
constexpr bool contains_type_v = contains_type<Search, Types...>::value;*/

template <typename Search, typename... Args>
struct contains_type;

template <typename Search>
struct contains_type<Search> : std::false_type {};

template <typename Search, typename ...Args>
struct contains_type<Search, Search, Args...> : std::true_type {};

template <typename Search, typename First, typename ...Rest>
struct contains_type<Search, First, Rest...>
: public contains_type<Search, Rest...> {};

template <typename Search, typename... Args>
constexpr bool contains_type_v = contains_type<Search, Args...>::value;


template <template <typename> class Transform, typename Search, typename... Args>
struct contains_type_by
: public contains_type<Search, typename Transform<Args>::type...> {};

template <template <typename> class Transform, typename Search, typename... Args>
constexpr bool contains_type_by_v = contains_type_by<Transform, Search, Args...>::value;



template <typename Search, typename Tup>
struct tuple_contains_type;

template <typename Search, template <typename...> class Tup, typename ...Args>
struct tuple_contains_type<Search, Tup<Args...>>
: public contains_type<Search, Args...> {};

template <typename Search, typename Tup>
constexpr bool tuple_contains_type_v = tuple_contains_type<Search, Tup>::value;


template <template <typename> class Transform, typename Search, typename Tup>
struct tuple_contains_type_by;

template <template <typename> class Transform, typename Search, template <typename...> class Tup, typename ...Args>
struct tuple_contains_type_by<Transform, Search, Tup<Args...>>
: public contains_type_by<Transform, Search, Args...> {};

template <template <typename> class Transform, typename Search, typename Tup>
constexpr bool tuple_contains_type_by_v = tuple_contains_type_by<Transform, Search, Tup>::value;


// ------------------------------------------------------------------------- //


template <typename OldType, typename NewType, typename Tup>
struct tuple_replace;

template <typename OldType, typename Tup>
struct tuple_replace<OldType, OldType, Tup> {
  using type = Tup;
};

template <typename OldType, typename NewType, template <typename...> class Tup, typename ...Types>
struct tuple_replace<OldType, NewType, Tup<Types...>> {
  using type = Tup<_if_t<std::is_same_v<Types, OldType>, NewType, Types>...>;
};

template <typename OldType, typename NewType, typename Tup>
using tuple_replace_t = typename tuple_replace<OldType, NewType, Tup>::type;


template <typename Tup0, typename Tup1>
struct concat_tuples;

// template <template <typename...> class Tup, typename ...Args0, typename ...Args1>
// struct concat_tuples<Tup<Args0...>, Tup<Args1...>> {
//   using type = Tup<Args0..., Args1...>;
// };

template <
  template <typename...> class Tup0,
  template <typename...> class Tup1,
  typename ...Args0,
  typename ...Args1
>
struct concat_tuples<Tup0<Args0...>, Tup1<Args1...>> {
  using type = Tup0<Args0..., Args1...>;
};

template <typename Tup0, typename Tup1>
using concat_tuples_t = typename concat_tuples<Tup0, Tup1>::type;


// ------------------------------------------------------------------------- //


template <typename Tup>
struct empty_from_types;

template <template <typename...> class Tup, typename ...Args>
struct empty_from_types<Tup<Args...>> {
  using type = Tup<>;
};

template <typename Tup>
using empty_from_types_t = typename empty_from_types<Tup>::type;


template <typename Tup, typename ...NewTypes>
struct push_back_type;

template <template <typename...> class Tup, typename ...Args, typename ...NewTypes>
struct push_back_type<Tup<Args...>, NewTypes...> {
  using type = Tup<Args..., NewTypes...>;
};

template <typename Tup, typename ...NewTypes>
using push_back_type_t = typename push_back_type<Tup, NewTypes...>::type;


template <typename Tup, typename ...NewTypes>
struct push_front_type;

template <template <typename...> class Tup, typename ...Args, typename ...NewTypes>
struct push_front_type<Tup<Args...>, NewTypes...> {
  using type = Tup<NewTypes..., Args...>;
};

template <typename Tup, typename ...NewTypes>
using push_front_type_t = typename push_front_type<Tup, NewTypes...>::type;


template <typename Tup, typename RESULT = empty_from_types_t<Tup>>
struct pop_back_type;

template <template <typename...> class Tup, typename LastType, typename RESULT>
struct pop_back_type<Tup<LastType>, RESULT> {
  using type = RESULT;
};

template <template <typename...> class Tup, typename T0, typename ...T1toN, typename RESULT>
struct pop_back_type<Tup<T0, T1toN...>, RESULT>
: pop_back_type<Tup<T1toN...>, push_back_type_t<RESULT, T0>> {};

template <typename Tup>
using pop_back_type_t = typename pop_back_type<Tup>::type;


template <typename Tup>
struct pop_front_type;

template <template <typename...> class Tup, typename FirstType, typename ...Rest>
struct pop_front_type<Tup<FirstType, Rest...>> {
  using type = Tup<Rest...>;
};

template <typename Tup>
using pop_front_type_t = typename pop_front_type<Tup>::type;




// ------------------------------------------------------------------------- //


template <typename Tup>
struct remove_ref_from_tuple;

template <template <typename...> class Tup, typename ...Args>
struct remove_ref_from_tuple<Tup<Args&...>> {
  using type = Tup<Args...>;
};

template <typename Tup>
using remove_ref_from_tuple_t = typename remove_ref_from_tuple<Tup>::type;


template <typename Tup>
struct remove_cvref_from_tuple;

template <template <typename...> class Tup, typename ...Args>
struct remove_cvref_from_tuple<Tup<Args...>> {
  using type = Tup<std::remove_cvref_t<Args>...>;
};

template <typename Tup>
using remove_cvref_from_tuple_t = typename remove_cvref_from_tuple<Tup>::type;


// ======================================================================= //


template <typename Container, typename = void>
struct is_map_container
: std::false_type {};

template <typename Container>
struct is_map_container<
  Container,
  std::void_t<
    typename Container::key_type,
    typename Container::mapped_type
  >
> : std::true_type {};

// Is a type with std::map interface
template <typename Container>
constexpr bool is_map_container_v = is_map_container<Container>::value;


template <typename Container, typename = void>
struct is_set_container
: std::false_type {};

template <typename Container>
struct is_set_container<
  Container,
  std::void_t<
    typename Container::key_type,
    typename Container::value_type
  >
> {
  static constexpr bool value = std::is_same_v<
    typename Container::key_type,
    typename Container::value_type
  >;
};

// Is a type with std::set interface
template <typename Container>
constexpr bool is_set_container_v = is_set_container<Container>::value;


template <typename Container, typename = void>
struct is_vector_container
: std::false_type {};

template <typename Container>
struct is_vector_container<
  Container,
  std::void_t<typename Container::iterator>
> {
  static_assert(is_iterator_v<typename Container::iterator>);
  static constexpr bool value = std::is_base_of_v<
    std::random_access_iterator_tag,
    typename std::iterator_traits<typename Container::iterator>::iterator_category
  >;
};

// Is a type with std::vector interface
template <typename Container>
constexpr bool is_vector_container_v = is_vector_container<Container>::value;


// ======================================================================= //

// template <typename ...Ts>
// struct are_duplicated_types;

template <typename ...Ts>
struct are_unique_types;

template <>
struct are_unique_types<> {
  static constexpr bool value = true;
};

template <typename T0, typename ...T1toN>
struct are_unique_types<T0, T1toN...> {
  static constexpr bool value =
    !contains_type_v<T0, T1toN...> &&
    are_unique_types<T1toN...>::value;
};

template <typename ...Ts>
constexpr bool are_unique_types_v = are_unique_types<Ts...>::value;


template <typename TupType>
struct are_tuple_elements_unique;

template <template <typename...> class TupType, typename ...Ts>
struct are_tuple_elements_unique<TupType<Ts...>>
: public are_unique_types<Ts...> {};

template <typename TupType>
constexpr bool are_tuple_elements_unique_v = are_tuple_elements_unique<TupType>::value;


// =========================================================== //


/*template <typename T, typename... Args>
struct is_convertible_constructible {
private:
  template <typename U, typename... UArgs>
  static auto test(U* ptr, UArgs&&... args)
  -> decltype(U(args...), std::true_type{});
  // -> decltype(U(std::forward<UArgs>(args)...), std::true_type{}); // std::is_constructible

  template <typename, typename...>
  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test<T>(nullptr, std::declval<Args>()...))::value;
};

template <typename T, typename... Args>
constexpr bool is_convertible_constructible_v = is_convertible_constructible<T, Args...>::value;


template <typename T, typename TupType>
struct can_convert_make_from_tuple;

template <typename T, template<typename...> class TupType, typename ...Args>
struct can_convert_make_from_tuple<T, TupType<Args...>>
: public is_convertible_constructible<T, Args...> {};

template <typename T, typename TupType>
constexpr bool can_convert_make_from_tuple_v = can_convert_make_from_tuple<T, TupType>::value;


// template <typename T, typename Tuple>
// constexpr T convert_make_from_tuple(Tuple&& tup) {
//   return std::apply(
//     [](auto&&... args) -> T {
//       return T(std::forward<decltype(args)>(args)...);
//       // return T(args...);
//     },
//     std::forward<Tuple>(tup)
//   );
// }
*/


template <typename TupType>
struct forward_as_tuple_type;

template <template <typename...> class TupType, typename ...Args>
struct forward_as_tuple_type<TupType<Args...>> {
  using type = TupType<Args&&...>;
};

template <typename TupType>
using forward_as_tuple_type_t = typename forward_as_tuple_type<TupType>::type;


// template <typename... Tuples>
// struct tuple_cat_type {
//   using type = decltype(std::tuple_cat(std::declval<Tuples>()...));
// };
//
// template <typename... Tuples>
// using tuple_cat_type_t = typename tuple_cat_type<Tuples...>::type;


template <typename ...Tuples>
struct tuple_cat_type;

template <typename Tuple>
struct tuple_cat_type<Tuple> {
  using type = Tuple;
};

template <template <typename...> class TupleType, typename ...Members0, typename ...Members1, typename ...Tuples2toN>
struct tuple_cat_type<TupleType<Members0...>, TupleType<Members1...>, Tuples2toN...>
: public tuple_cat_type<TupleType<Members0..., Members1...>, Tuples2toN...> {};

template <typename ...Tuples>
using tuple_cat_type_t = typename tuple_cat_type<Tuples...>::type;


// template <typename T>
// constexpr T&& my_forward(std::remove_reference_t<T>& arg) noexcept {
//   return static_cast<T&&>(arg);
// }
//
// template <typename T>
// constexpr T&& my_forward(std::remove_reference_t<T>&& arg) noexcept {
//   static_assert(!std::is_lvalue_reference_v<T>,
//                 "Bad forward of rvalue as lvalue");
//   return static_cast<T&&>(arg);
// }


// Primary template: default to false
template<typename T, typename = void>
struct is_unary_function
: public std::false_type {};

// Specialization for a any functor class (including lambdas through std::function) 
// BUG: This is NOT correct, because it works with any std::function signature, not only unary
// and it also doesnt work with functor classes, because it always gives false for them
template <typename T>
struct is_unary_function<T, std::void_t<decltype(&T::operator())>>
: public std::true_type {};

// template <typename T, typename Arg>
// struct is_unary_function<T, std::void_t<decltype(std::declval<T>()(std::declval<Arg>()))>>
// : public std::true_type {};

// Specialization for function pointers with one argument
template<typename Ret, typename Arg>
// struct is_unary_function<Ret(*)(Arg)>
struct is_unary_function<Ret(Arg)>
: public std::true_type {};

// Specialization for member-function pointers with one argument
// static_assert(is_unary_function_v<decltype(&Foo::foo)>);
template<typename Class, typename Ret, typename Arg>
struct is_unary_function<Ret(Class::*)(Arg)>
: public std::true_type {};

// Specialization for const member-function pointers with one argument
template<typename Class, typename Ret, typename Arg>
struct is_unary_function<Ret(Class::*)(Arg) const>
: public std::true_type {};

// Convenience variable template (C++14+)
template<typename T>
constexpr bool is_unary_function_v = is_unary_function<T>::value;


/*// Base template: false by default
template<typename T, typename = void>
struct is_unary_callable : std::false_type {};

// Specialization for function pointers (free functions)
template<typename Ret, typename Arg>
struct is_unary_callable<Ret(*)(Arg)>
: std::integral_constant<bool, true> {};

// Specialization for non-const member function pointers (1 argument)
template<typename Ret, typename Class, typename Arg>
struct is_unary_callable<Ret(Class::*)(Arg)>
: std::integral_constant<bool, true> {};

// Specialization for const member function pointers (1 argument)
template<typename Ret, typename Class, typename Arg>
struct is_unary_callable<Ret(Class::*)(Arg) const>
: std::integral_constant<bool, true> {};

// Specialization for functors and lambdas (with operator())
// template<typename T>
// struct is_unary_callable<T, std::void_t<decltype(&T::operator())>>
//     : std::is_invocable_r<void, T, typename std::tuple_element<0, std::tuple<typename T::operator()>>::type> {};

// Specialization for functors and lambdas (with operator())
// template<typename T>
// struct is_unary_callable<T, std::void_t<decltype(&T::operator())>>
// {
//     // Check if operator() takes exactly one argument
//     using result = decltype(&T::operator());
//     using invoc_type = std::invoke_result_t<result, T, typename std::tuple_element<0, std::tuple<typename T::operator()>>::type>;
//
//     static constexpr bool value = std::is_invocable_v<result, T, typename std::tuple_element<0, std::tuple<typename T::operator()>>::type>;
// };*/


// ================================================================ //

// NOTE: Those may be needed in jump_iterator.hpp
/*
// template <typename T, typename = void>
// struct does_have_base : std::false_type {};
//
// template <typename T>
// struct does_have_base<T, std::void_t<typename T::base_type>>
// : public std::true_type {};
//
// template <typename T>
// constexpr bool does_have_base_v = does_have_base<T>::value;

// template <typename T, size_t DEPTH = 1>
// struct does_have_base_depth;
//
// template <typename T, size_t DEPTH>
// struct does_have_base_depth {
//  static constexpr bool value = does_have_base_v<T> && does_have_base_depth<
// };
//
// template <typename T, size_t DEPTH>
// constexpr bool does_have_base_depth_v = does_have_base<T>::value;

template <typename T, size_t DEPTH = 1, typename = void>
struct does_have_base_typedef : std::false_type {};

template <typename T, size_t DEPTH>
struct does_have_base_typedef<T, DEPTH, std::void_t<typename T::base_type>>
: public does_have_base_typedef<typename T::base_type, DEPTH - 1> {};

template <typename T>
struct does_have_base_typedef<T, 0, void>
: public std::true_type {};

template <typename T, size_t DEPTH = 1>
constexpr bool does_have_base_typedef_v = does_have_base_typedef<T, DEPTH>::value;


template <typename T, size_t DEPTH = 1, typename = void>
struct does_have_base_method : std::false_type {};

template <typename T, size_t DEPTH>
struct does_have_base_method<T, DEPTH, std::void_t<decltype(std::declval<T>().base())>>
: public does_have_base_method<decltype(std::declval<T>().base()), DEPTH - 1> {};

template <typename T>
struct does_have_base_method<T, 0, void>
: public std::true_type {};

template <typename T, size_t DEPTH = 1>
constexpr bool does_have_base_method_v = does_have_base_method<T, DEPTH>::value;

// NOTE: Example Usage
//
// using iter_type0 = index_iterator<int>;
// using iter_type1 = jump_iterator<iter_type0, NoPolicy<>>;
// using iter_type2 = jump_iterator<iter_type1, NoPolicy<>>;
// using iter_type3 = jump_iterator<iter_type2, NoPolicy<>>;
// using iter_type4 = jump_iterator<iter_type3, NoPolicy<>>;
//
// static_assert(std::is_same_v<get_base_type_t<iter_type1>, iter_type0>);
// static_assert(std::is_same_v<get_base_type_t<iter_type2, 2>, iter_type0>);
// static_assert(std::is_same_v<get_base_type_t<iter_type3, 3>, iter_type0>);
// static_assert(std::is_same_v<get_base_type_t<iter_type4, 4>, iter_type0>);
// static_assert(does_have_base_v<iter_type3, 3>);


// template <typename T, typename = void>
// struct has_nonconst_base
// : public std::false_type {};
//
// template <typename T>
// class has_nonconst_base<
//  T,
//  // std::void_t<decltype(std::declval<T&>().base())>
//  std::void_t<decltype(std::declval<T>().base())>
// > {
//  using result_type = decltype(std::declval<T>().base());
// public:
//  static constexpr bool value = 
//    std::is_reference_v<result_type> &&
//    !std::is_const_v<std::remove_reference_t<result_type>>;
// };
//
// template <typename T>
// constexpr bool has_nonconst_base_v = has_nonconst_base<T>::value;


template <typename T, size_t DEPTH>
struct get_base_typedef
: get_base_typedef<typename T::base_type, DEPTH - 1> {};

template <typename T>
struct get_base_typedef<T, 0> {
  using type = T;
};

template <typename T, size_t DEPTH = 1>
using get_base_typedef_t = typename get_base_typedef<T, DEPTH>::type;
*/

// ================================================================ //


// Metafunction to convert function type to function pointer
// NOTE: This is one of the three type conversions that std::decay does
template <typename T>
struct function_to_pointer { using type = T; };

template <typename R, typename... Args>
struct function_to_pointer<R(Args...)> {
    using type = R(*)(Args...);
};

template <typename T>
using function_to_pointer_t = typename function_to_pointer<T>::type;



template <typename T, bool = std::is_integral_v<T>>
struct safe_make_signed;

template <typename T>
struct safe_make_signed<T, false> {
  using type = T;
};

template <typename T>
struct safe_make_signed<T, true>
: public std::make_signed<T> {};

template <typename T>
using safe_make_signed_t = typename safe_make_signed<T>::type;


/*template <typename Range, typename = void>
struct range_to_iterator {
  static_assert(is_range_v<Range>);
  using type = typename Range::iterator;
};

template <typename Range>
struct range_to_iterator<
  Range,
  std::void_t<
    typename Range::iterator,
    typename Range::const_iterator
  >
> {
  static_assert(is_range_v<Range>);
  using type = _if_t<
    std::is_const_v<Range>,
    typename Range::const_iterator,
    typename Range::iterator
  >;
};*/

template <typename Range>
struct range_to_iterator {
  using type = std::remove_cvref_t<decltype(std::declval<Range>().begin())>;
};

template <typename Range>
using range_to_iterator_t = typename range_to_iterator<Range>::type;




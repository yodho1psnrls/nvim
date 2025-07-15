#pragma once

#include "../meta_func.hpp"
#include "key_traits.hpp"

#include <vector>
#include <unordered_map>


// NOTE: A library for fusing additional elements per element of some container
// so for example if you are given a VecType = std::vector<int>, you can generically use
// fuse_out_t<VecType, float> and the result is PropContainer<std::vector<int>, std::vector<float>>.
// Or fuse_in_t<VecType, float> will result in std::vector<std::tuple<int, float>>.
// And you also get helper conversion functions

// NOTE: Examples:
// fuse_out_t<vector<int>, float> == PropCont<vector<int>, vector<float>>
// fuse_out_t<list<int>, float> == PropCont<list<int>, hash_map<list::iter, float>>
// fuse_in_t<vector<int>, float> == vector<std::tuple<int, float>>.
// fuse_in_t<list<int>, float> == list<std::tuple<int, float>>.



// Default property container
template <typename Cont, typename T>
using cont_get_property_t = _if_t<
	is_vector_container_v<Cont>,
	std::vector<T>, 
	std::unordered_map<typename key_cont_traits<Cont>::key_type, T>
>;


template <
	typename Cont,
	typename Prop,
	bool VEC = is_vector_container_v<Cont>
> struct can_be_property;

template <typename Cont, typename Prop>
struct can_be_property<Cont, Prop, true> {
	static constexpr bool value =
		is_vector_container_v<Prop>
		|| is_map_container_v<Prop>;
};

template <typename Cont, typename Prop>
struct can_be_property<Cont, Prop, false> {
	static_assert(is_container_v<Cont>);
	static constexpr bool value =
		is_map_container_v<Prop>;
};

template <typename Cont, typename Prop>
constexpr bool can_be_property_v = can_be_property<Cont, Prop>::value;








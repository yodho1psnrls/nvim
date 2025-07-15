#pragma once
#include "crtp_iterator.hpp"


struct GetBaseFunctor {

	template <typename IterType>
	typename IterType::base_type& operator()(IterType& it) const {
		return it.base();
	}
	
	template <typename IterType>
	const typename IterType::base_type& operator()(const IterType& it) const {
		return it.base();
	}

};



template <typename IterType>
class BaseIterator
: public CrtpIterator<BaseIterator<IterType>, IterType, GetBaseFunctor> {

	using ThisType = BaseIterator<IterType>;
	using CrtpType = CrtpIterator<ThisType, IterType, GetBaseFunctor>;
	using BaseIter = typename CrtpType::template IterType<0>;

public:

	using CrtpType::CrtpType;

	// using original_type = 

	using base_type = IterType;
	const IterType& base() const { return CrtpType::base(); }


	template <typename IT>
	BaseIterator(const BaseIterator<IT>& other)
	: CrtpType(other.base()) {}


};

template <typename IterType>
BaseIterator(const IterType&)
-> BaseIterator<IterType>;



#pragma once

// #include "../iterators/filter_iterator.hpp"
//
// #include <initializer_list>
// #include <stdexcept>
// #include <vector>

#include "unordered_vector.hpp"
#include "garbage_vector.hpp"

// NOTE: This container can be modeled as 
// specialization of GarbageVector<UnorderedVector<...>>


// NOTE: Stable Index Vector
// holding an index to an element,
// the index is valid until the value that the
// index points to is erased, or if the container
// is compacted(collect the garbaged elements)
// The elements are not ordered as in std::vector sequentially
// by the order and position that you insert them
// Insert and Erase are const time(for one element) or O(n)
// where n is NOT the size of the vector, but the number
// of elements inserted


template <typename T, typename Alloc = std::allocator<T>>
using StableVector = GarbageVector<UnorderedVector<std::vector<T, Alloc>>>;


/*template <typename T, typename Allocator = std::allocator<T>>
// template <typename VecType>
class StableVector {

	// static_assert(std::is_base_of_v<
	// 	std::random_access_iterator_tag,
	// 	typename std::iterator_traits<typename VecType::iterator>::iterator_category
	// >);
	
	using VecType = std::vector<T, Allocator>;

	VecType _vec;
	std::vector<bool, std::allocator<bool>> _erased_mask;
	size_t _erased_count;

public:

	using size_type = typename VecType::size_type;
	using difference_type = typename VecType::difference_type;

	using value_type = typename VecType::value_type;
	using reference = typename VecType::reference;
	using const_reference = typename VecType::const_reference;
	using pointer = typename VecType::pointer;
	using const_pointer = typename VecType::const_pointer;

	using iterator = FilterIterator<typename VecType::iterator, std::vector<bool>::const_iterator, false>;
	using const_iterator = FilterIterator<typename VecType::const_iterator, std::vector<bool>::const_iterator, false>;

	VecType& base() { return _vec; }
	const VecType& base() const { return _vec; }

	StableVector()
	: _vec()
	,	_erased_mask()
	, _erased_count(0) {}

	StableVector(size_type n, const value_type& val = value_type())
	: _vec(n, val)
	,	_erased_mask(n, false)
	, _erased_count(0) {}
	
	StableVector(std::initializer_list<value_type> l)
	: _vec(l)
	,	_erased_mask(l.size(), false)
	, _erased_count(0) {}

	template <typename IterType>
	StableVector(IterType first, IterType last)
	: _vec(first, last)
	,	_erased_mask(_vec.size())
	, _erased_count(0) {}

	void assign(size_type n, const value_type& val = value_type()) {
		_vec.assign(n, val);
		_erased_mask.assign(n, false);
		_erased_count = 0;
	}
	
	void assign(std::initializer_list<value_type> l) {
		_vec.assign(l);
		_erased_mask.assign(l.size(), false);
		_erased_count = 0;
	}

	template <typename IterType>
	void assign(IterType first, IterType last) {
		_vec.assign(first, last);
		_erased_mask.assign(_vec.size(), false);
		_erased_count = 0;
	}

	// ---------------------------------------------------------------------- //
  
	bool is_compact() const {
    return !_erased_count;
  }
	
	// void compact() {
	// 	if (is_compact())
	// 		return;
	//
	// 	// for(auto it = base_type::rbegin(); it != base_type::rend(); ++it)
	// 	// 	if (is_erased())
	// 	// 		base_type::erase(it.base());
	//
	// 	for(difference_type i = _vec.size() - 1; i != -1; --i)
	// 		if(is_erased(i)) {
	// 			_vec.erase(_vec.cbegin() + i);
	// 			// _erased_mask[i] = false;	// They should be separate
	// 		}
	//
	// 	_erased_mask.assign(_erased_mask.size(), false);
	// 	_erased_count = 0;
	// }

	void compact(difference_type id) {
		if (!is_erased(id))
			throw std::invalid_argument("The element which you want to compact should be marked as erased beforehand");
				
		// _erased_mask[i] = false;	// They should be separate
		// _vec.erase(_vec.cbegin() + i);

		// _erased_count -= is_erased(id);
		--_erased_count;

		if constexpr (std::is_move_assignable_v<reference>) {
			_vec[id] = std::move(_vec.back());
			_erased_mask[id] = std::move(_erased_mask.back());
		}
		else {
			_vec[id] = _vec.back();
			_erased_mask[id] = _erased_mask.back();
		}

		_vec.pop_back();
		_erased_mask.pop_back();
	}

	void compact() {
		if (is_compact())
			return;

		for(difference_type i = _vec.size() - 1; i != -1; --i)
			if(_erased_mask[i]) {
				if constexpr (std::is_move_assignable_v<reference>)
					_vec[i] = std::move(_vec.back());
				else
					_vec[i] = _vec.back();
				_vec.pop_back();
			}

		_erased_mask.assign(_vec.size(), false);
		_erased_count = 0;
	}

  size_t erased_size() const {
    return _erased_count;
  }

  bool is_erased(size_type id) const {
    return _erased_mask[id];
  }
  
	// bool is_erased(const_iterator it) const {
  //   return _erased_mask[it - _vec.cbegin()];
  // }
	
	// ---------------------------------------------------------------------- //
	
	bool empty() const {
		// return _vec.empty();
		return _vec.size() == _erased_count;
	}

	size_type size() const {
		// return _vec.size();
		return _vec.size() - _erased_count;
	}
	
	// void resize(size_type n) {
	// 	// if (n < _erased_mask.size() && _erased_count)
	// 	// 	for(auto it = _erased_mask.begin() + n; it != _erased_mask.end(); ++it)
	// 	if (_erased_count)
	// 		for(auto it = _erased_mask.begin() + n; it < _erased_mask.end(); ++it)
	// 			_erased_count -= *it;
	//
	// 	_vec.resize(n);
	// 	_erased_mask.resize(n, false);
	// }
	
	size_type capacity() const {
		return _vec.capacity();
	}

	void reserve(size_type n) {
		_vec.reserve(n);
		_erased_mask.reserve(n);
	}

	// ------------------------------------------------------------------- //
	
	difference_type idx(const_iterator it) const {
	// difference_type iter_to_id(const_iterator it) const {
		return it.base() - _vec.cbegin();
	}

	iterator find(difference_type id) {
	// iterator id_to_iter(difference_type id) {
		if (is_erased(id))
			throw std::invalid_argument("Cannot get the iterator of an erased element");
		return iterator(_vec.begin() + id, _erased_mask.cbegin() + id);
	}
	
	const_iterator find(difference_type id) const {
	// const_iterator id_to_iter(difference_type id) const {
		if (is_erased(id))
			throw std::invalid_argument("Cannot get the iterator of an erased element");
		return const_iterator(_vec.cbegin() + id, _erased_mask.cbegin() + id);
	}

	// Returns a view which indicates how each index for the elements is offsetted	
	auto offsets() const {

	}
	
	// ------------------------------------------------------------------- //

	reference operator[](difference_type i) {
		if (is_erased(i))
			throw std::invalid_argument("Cannot acess erased/invalidated element");

		return _vec[i];
	}

	const_reference operator[](difference_type i) const {
		if (is_erased(i))
			throw std::invalid_argument("Cannot acess erased/invalidated element");

		return _vec[i];
	}
	
	reference at(difference_type i) {
		if (is_erased(i))
			throw std::invalid_argument("Cannot acess erased/invalidated element");

		return _vec.at(i);
	}

	const_reference at(difference_type i) const {
		if (is_erased(i))
			throw std::invalid_argument("Cannot acess erased/invalidated element");

		return _vec.at(i);
	}
	
	// ---------------------------------------------------------------------- //

	// reference front() {
	// 	if (empty())
	// 		throw std::invalid_argument("Cannot get the front of an empty container");
	//
	// 	auto it = _vec.begin();
	// 	auto eit = _erased_mask.cbegin();
	//
	// 	while(*eit) {
	// 		++eit;
	// 		++it;
	// 	}
	//
	// 	return *it;
	// }
	// 
	// const_reference front() const {
	// 	if (empty())
	// 		throw std::invalid_argument("Cannot get the front of an empty container");
	//
	// 	auto it = _vec.cbegin();
	// 	auto eit = _erased_mask.cbegin();
	//
	// 	while(*eit) {
	// 		++eit;
	// 		++it;
	// 	}
	//
	// 	return *it;
	// }
	// 
	// reference back() {
	// 	if (empty())
	// 		throw std::invalid_argument("Cannot get the front of an empty container");
	//
	// 	auto it = std::prev(_vec.end());
	// 	auto eit = std::prev(_erased_mask.cbegin());
	//
	// 	while(*eit) {
	// 		--eit;
	// 		--it;
	// 	}
	//
	// 	return *it;
	// }
	// 
	// const_reference back() const {
	// 	if (empty())	// if _vec.size() == _erased_count
	// 		throw std::invalid_argument("Cannot get the front of an empty container");
	//
	// 	auto it = std::prev(_vec.cend());
	// 	auto eit = std::prev(_erased_mask.cbegin());
	//
	// 	while(*eit) {
	// 		--eit;
	// 		--it;
	// 	}
	//
	// 	return *it;
	// }
	
	// ---------------------------------------------------------------------- //
	
	iterator begin() {
		return iterator(
			_vec.begin(),
			_vec.end(),
			_erased_mask.cbegin()
		);
	}
	
	iterator end() {
		return iterator(
			_vec.end(),
			_vec.end(),
			_erased_mask.cend()
		);
	}
	
	const_iterator begin() const {
		return const_iterator(
			_vec.begin(),
			_vec.end(),
			_erased_mask.cbegin()
		);
	}
	
	const_iterator end() const {
		return const_iterator(
			_vec.end(),
			_vec.end(),
			_erased_mask.cend()
		);
	}
	
	const_iterator cbegin() const {
		return const_iterator(
			_vec.cbegin(),
			_vec.cend(),
			_erased_mask.cbegin()
		);
	}
	
	const_iterator cend() const {
		return const_iterator(
			_vec.cend(),
			_vec.cend(),
			_erased_mask.cend()
		);
	}
	
	// ---------------------------------------------------------------------- //

	// returns the id of the pushed value
	difference_type push(const value_type& val) {
		if (is_compact()) {
			difference_type i = size();
			// _vec.insert(_vec.end(), val);
			_vec.push_back(val);
			return i;
		}
		else {
			difference_type i = 0;
			while(!_erased_mask[i])
				++i;

			_erased_mask[i] = false;
			--_erased_count;
					
			_vec[i] = val;
			return i;
		}
	}
	
	difference_type push(value_type&& val) {
		if (is_compact()) {
			difference_type i = size();
			_vec.push_back(std::move(val));
			return i;
		}
		else {
			difference_type i = 0;
			while(!_erased_mask[i])
				++i;

			_erased_mask[i] = false;
			--_erased_count;

			_vec[i] = std::move(val);
			return i;
		}
	}

	void pop(difference_type id) {
		if (_erased_mask[id])
			throw std::invalid_argument("Cannot erase already erased element");

		_erased_mask[id] = true;
		++_erased_count;
	}

};*/



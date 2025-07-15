#pragma once

// #include "../keys/keys.hpp"
// #include "../key_cont/key_update_traits.hpp"

#include "../iterators/lookup_iterator.hpp"
#include "../iterators/tensor_iterator.hpp"

#include "../iterators/index_iterator.hpp"
#include "../iterators/iterator_iterator.hpp"

#include "../iterators/flat_iterator.hpp"

// NOTE: 3 Kinds of Policies:
// (*) Flat Contiguous (the end of the current row is the begin of the next row)
// (*) Row only linear (the end of the current row is an invalid key, like -1 or nullptr)
// (*) Cyclyc rows (the end of the current row is the begin of the current row)

template <typename IndexIter>
class CycleByIndexValue {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using policy_category = nonfixed_jump_policy_tag;

  // JumpByIndexValue() = default;
  CycleByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}
  // __JumpByIndexValue(const IndexIter& index_it) : _id_it(index_it) {}

  const IndexIter& iter() const { return _id_it; }
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


/*template <typename IndexIter>
class JumpByLookup {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

  static_assert(is_iterator_v<IndexIter>);
  static_assert(std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<IndexIter>::iterator_category>);
  // static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

  IndexIter _id_it;

public:
  
  using difference_type = typename std::iterator_traits<IndexIter>::difference_type;
  // using iterator_category = base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using iterator_category_result = most_base_t<IC<IndexIter>, std::forward_iterator_tag>;
  using required_iterator_category = void;
  using policy_category = nonfixed_jump_policy_tag;

  JumpByLookup(const IndexIter& index_it) : _id_it(index_it) {}
  
  // template <typename IT>
  // JumpByLookup(const JumpByLookup<IT>& other) : _id_it(other.base()) {}

  const IndexIter& iter() const { return _id_it; }
  difference_type value() const { return *_id_it; }

public:
 
  // void advance_alone() { _id_it += *_id_it; }

  // template <typename T>
  // void advance(index_iterator<T>& it) const {
  //   it = index_iterator(_id_it[*it]);
  // }
 
	template <typename T>
  void advance(IndexIterator<T>& it) const {
    it = _id_it[*it];
    // it = IT(_id_it[*it]);
  }
	
	// template <typename IT, typename LT>
  // void advance(LookupIterator<IT, LT>& it) const {
  //	 it = _id_it[*(it.base())];
  //	 it = _id_it[it.key()];
  //	 it = _id_it[*it.functor().lookup()];
	// }

};*/

template <typename LookupType>
class JumpByLookup {
  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

	static_assert(is_container_v<LookupType> || is_iterator_v<LookupType>);
  // static_assert(std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<IndexIter>::iterator_category>);
  // static_assert(std::is_integral_v<typename std::iterator_traits<IndexIter>::value_type>);

	LookupFunctor<LookupType> _look_func;

public:
  
  using difference_type = typename LookupFunctor<LookupType>::difference_type;
  using iterator_category_result = std::forward_iterator_tag;
  using required_iterator_category = void;
  using policy_category = nonfixed_jump_policy_tag;

  JumpByLookup(const LookupFunctor<LookupType>& lookup)
	: _look_func(lookup) {}

	template <typename LT>
  JumpByLookup(const JumpByLookup<LT>& other)
	: _look_func(other.lookup()) {}

	// NOTE: This is NOT a random acess iterator,
	//  so we dont need the iter() for operator-
  // const IndexIter& iter() const { return _id_it; }

	// NOTE: Neither it has a value(), because this doesnt make sense
	// , since its a lookup that doesnt change, not as the other
	// nonfixed jump policies, which advance and change its value()
  // difference_type value() const { return *_id_it; }
	
	using lookup_type = LookupType;
	lookup_type&& lookup() const {
		return _look_func.lookup();
	}

public:
 
	template <typename T>
  void advance(IndexIterator<T>& it) const {
    it = _look_func(*it);
  }

};


// template <std::ptrdiff_t EQUAL_ID>
template <typename EQUAL_TYPE, EQUAL_TYPE EQUAL_VALUE>
class JumpByEqualStatic {

  template <typename IT>
  using IC = typename std::iterator_traits<IT>::iterator_category;

public:
  
  using difference_type = std::ptrdiff_t;
  // using iterator_category = std::forward_iterator_tag;
  using iterator_category_result = void;
  using required_iterator_category = void;
  using policy_category = static_jump_policy_tag;

  JumpByEqualStatic() = default;

  static constexpr EQUAL_TYPE value() { return EQUAL_VALUE; }

public:

  template <typename IterType>
  static void advance(IterType& it) {
    // static_assert(std::is_integral_v<T>);
    // it = index_iterator<T>(EQUAL_VALUE);
    it = IterType(EQUAL_VALUE);
  }

};


// ================================================================== //

// BUG: insert on an empty matrix, crashes

template <typename ContType, bool LINEAR = true>
class CyclicMatrix {

  static_assert(is_container_v<ContType>);
  static_assert(std::is_base_of_v<std::random_access_iterator_tag,
    typename std::iterator_traits<typename ContType::iterator>::iterator_category>);

  using values_vec_type = ContType;
  using ranges_vec_type = std::vector<IndexIterator<typename ContType::difference_type>>;

public:

  using size_type = typename values_vec_type::size_type;
  using difference_type = typename values_vec_type::difference_type;
  using allocator_type = typename ContType::allocator_type;

  using flat_value_type = typename ContType::value_type;
  using flat_reference = typename values_vec_type::reference;
  using flat_const_reference = typename values_vec_type::const_reference;

  using flat_iterator = rec_jump_iterator_t<
		LookupIterator<IndexIterator<difference_type>, ContType>,
		// JumpByLookup<typename ranges_vec_type::const_iterator>
		JumpByLookup<const ranges_vec_type>
	>;

  using flat_const_iterator = rec_jump_iterator_t<
		LookupIterator<IndexIterator<difference_type>, const ContType>,
		// JumpByLookup<typename ranges_vec_type::const_iterator>
		JumpByLookup<const ranges_vec_type>
	>;

  using value_type = values_vec_type;

	using reference = TensorView<
		flat_iterator,
		JumpByEqualStatic<difference_type, -1>,
		false
	>;

	using const_reference = TensorView<
		flat_const_iterator,
		JumpByEqualStatic<difference_type, -1>,
		false
	>;

  using pointer = reference*;
  using const_pointer = const_reference*;

  using iterator = rec_jump_iterator_t<
		ViewToIterator<reference>,
		JumpByEqualNext<typename ranges_vec_type::const_iterator>
	>;

  using const_iterator = rec_jump_iterator_t<
		ViewToIterator<const_reference>,
		JumpByEqualNext<typename ranges_vec_type::const_iterator>
	>;

private:

  values_vec_type _vec;
  ranges_vec_type _row_to_vec;	// This should be a vector of integers
  ranges_vec_type _next_vec_in_row;	// This should be a vector of index iterators

	static inline IndexIterator<difference_type> END_ID
		= IndexIterator(difference_type(-1));
	

public:

	CyclicMatrix() {}

  CyclicMatrix(
    difference_type num_of_rows,
    difference_type num_of_cols,
    const flat_value_type& val = flat_value_type()
  ) : _vec(num_of_rows * num_of_cols, val) {

		_row_to_vec.reserve(num_of_rows);
		_next_vec_in_row.reserve(num_of_rows * num_of_cols);

		for(difference_type i = 0; i < num_of_rows; ++i) {
      _row_to_vec.push_back(IndexIterator(i * num_of_cols));

      _next_vec_in_row.insert(
				_next_vec_in_row.end(),
				IteratorIterator(IndexIterator(i * num_of_cols)) + 1,
				IteratorIterator(IndexIterator((i + 1) * num_of_cols)) + 1
			);

			if constexpr (LINEAR)
				_next_vec_in_row.back() = END_ID;
			else // Cyclic rows
				_next_vec_in_row.back() = *(_next_vec_in_row.end() - num_of_cols - 1);
    }

  }


	size_type size() const {
		return _row_to_vec.size();
	}
	
	bool empty() const {
		return _row_to_vec.empty();
	}

	void reserve(size_type n) {
		_row_to_vec.reserve(n);
	}

	void reserve_flat(size_type n) {
		_vec.reserve(n);
	}

	const values_vec_type& flat() const {
		return _vec;
	}


	reference operator[](difference_type i) {
		flat_iterator fit(
			make_lookup_iterator(_row_to_vec[i], _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(static_cast<const ranges_vec_type&>(_next_vec_in_row))
		);
    return reference(fit);
	}
	
	const_reference operator[](difference_type i) const {
		flat_const_iterator fit(
			make_lookup_iterator(_row_to_vec[i], _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(_next_vec_in_row)
		);
    return const_reference(fit);
	}

	reference at(difference_type i) {
		flat_iterator fit(
			make_lookup_iterator(_row_to_vec[i], _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(static_cast<const ranges_vec_type&>(_next_vec_in_row))
		);
    return reference(fit);
	}
	
	const_reference at(difference_type i) const {
		flat_const_iterator fit(
			make_lookup_iterator(_row_to_vec[i], _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(_next_vec_in_row)
		);
    return const_reference(fit);
	}
	

	iterator begin() {
		return iterator(ViewToIterator<reference>((*this)[0]), _row_to_vec.cbegin());
	}
	
	iterator end() {
		return iterator(ViewToIterator<reference>((*this)[size()]), _row_to_vec.cend());
	}

	const_iterator begin() const {
		return const_iterator(ViewToIterator<const_reference>((*this)[0]), _row_to_vec.cbegin());
	}
	
	const_iterator end() const {
		return const_iterator(ViewToIterator<const_reference>((*this)[size()]), _row_to_vec.cend());
	}
	
	const_iterator cbegin() const {
		return const_iterator(ViewToIterator<const_reference>((*this)[0]), _row_to_vec.cbegin());
	}
	
	const_iterator cend() const {
		return const_iterator(ViewToIterator<const_reference>((*this)[size()]), _row_to_vec.cend());
	}


	// --------------------------------------------------------------------------------------- //


	void new_row(const_iterator where) {
		typename ranges_vec_type::const_iterator new_it =
			_row_to_vec.insert(where.policy().iter(), IndexIterator(difference_type(-1)));
		// static_assert(std::is_same_v<std::remove_cvref_t<decltype(where.policy().iter())>, typename ranges_vec_type::const_iterator>);
		// return iterator(ViewToIterator((*this)[0LL]), new_it);
	}

	flat_iterator get_flat_iter(typename ranges_vec_type::const_iterator row_it) {
		return flat_iterator(
			make_lookup_iterator(*row_it, _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(static_cast<const ranges_vec_type&>(_next_vec_in_row))
		);
	}
	
	flat_const_iterator get_flat_iter(typename ranges_vec_type::const_iterator row_it) const {
		return flat_const_iterator(
			make_lookup_iterator(*row_it, _vec),
			// _next_vec_in_row.cbegin()
			make_lookup_functor(_next_vec_in_row)
		);
	}

	iterator get_row_iter(typename ranges_vec_type::const_iterator row_it) {
		flat_iterator flat_it = get_flat_iter(row_it);
		return iterator(ViewToIterator<reference>(reference(flat_it)), row_it);
	}
	
	const_iterator get_row_iter(typename ranges_vec_type::const_iterator row_it) const {
		flat_const_iterator flat_it = get_flat_iter(row_it);
		return const_iterator(ViewToIterator<const_reference>(const_reference(flat_it)), row_it);
	}

	// iterator get_row_iter(difference_type row_id) {
	// 	flat_iterator flat_it = get_flat_iter();
	// 	return iterator(
	// 		ViewToIterator<reference>(reference(flat_it)),
	// 		_row_to_vec.cbegin() + row_id
	// 	);
	// }


	// --------------------------------------------------------------------------------------- //
	
	// void push_back(const value_type& val) {
	//
	// }


	iterator insert(const_iterator where, const value_type& value) {
		auto ii = IteratorIterator(IndexIterator(difference_type(_next_vec_in_row.size())));

		if (value.empty()) {
			if constexpr (LINEAR)
				return get_row_iter(_row_to_vec.insert(where.policy().iter(), END_ID));
			else
				return get_row_iter(_row_to_vec.insert(where.policy().iter(), *ii));
		}

		typename ranges_vec_type::const_iterator row_it =
			_row_to_vec.insert(where.policy().iter(), *ii);

		auto flat_it = _vec.insert(_vec.end(), value.begin(), value.end());

		// +1 so they point to the next value
		_next_vec_in_row.insert(_next_vec_in_row.end(), ii + 1, ii + value.size() + 1);

		if constexpr (LINEAR)
			_next_vec_in_row.back() = END_ID;
		else
			_next_vec_in_row.back() = *(_next_vec_in_row.end() - value.size() - 1);
	
		return get_row_iter(row_it);
	}


	/*template <typename IT, typename = std::enable_if_t<is_iterator_v<IT>>>
	iterator insert(const_iterator where, IT first, IT last) {
		using IT_VT = typename std::iterator_traits<IT>::value_type;
		static_assert(is_range_v<IT_VT>);
		// static_assert(std::is_assignable_v<flat_reference, typename IT_VT::reference>);
		// static_assert(std::is_assignable_v<reference, typename std::iterator_traits<IT>::reference>);

		auto ii = IteratorIterator(IndexIterator(difference_type(_next_vec_in_row.size())));

		// difference_type flat_size = _vec.size();
		auto flat_it = _vec.insert(_vec.end(), make_flat_iterator(first), make_flat_iterator(last));
		difference_type flat_size = _vec.size() - *(*ii);

		// +1 so they point to the next value
		auto next_it = _next_vec_in_row.insert(_next_vec_in_row.end(), ii + 1, ii + flat_size + 1);

		// auto n = std::distance(first, last);
		// difference_type flat_id = *(*ii);
		difference_type row_num = 0;
		while(first != last) {
		// for(auto it = next_it; it != _next_vec_in_row.end(); ++it) {
			// TODO: LINEAR constexpr branching
			std::advance(next_it, first->size() - 1);
			*next_it = END_ID;
			++next_it;

			++first;
			++row_num;
			// flat_id += first->size();
		}

		auto cont_to_size = [](const auto& x){ return x.size(); };
		auto jii_beg = jump_iterator(ii, JumpByIndexValue(TransformIterator(first, cont_to_size)));
		auto jii_end = jump_iterator(ii + flat_size, JumpByIndexValue(TransformIterator(last, cont_to_size)));

		// NOTE: There is the problem
		typename ranges_vec_type::const_iterator row_it =
			// _row_to_vec.insert(where.policy().iter(), ii, ii + row_num);
			_row_to_vec.insert(where.policy().iter(), jii_beg, jii_end);

		return get_row_iter(row_it);
	}*/
	
	template <typename IT, typename = std::enable_if_t<is_iterator_v<IT>>>
	iterator insert(const_iterator where, IT first, IT last) {
		using IT_VT = typename std::iterator_traits<IT>::value_type;
		static_assert(is_range_v<IT_VT>);
		// static_assert(std::is_assignable_v<flat_reference, typename IT_VT::reference>);
		// static_assert(std::is_assignable_v<reference, typename std::iterator_traits<IT>::reference>);

		auto ii = IteratorIterator(IndexIterator(difference_type(_next_vec_in_row.size())));

		// difference_type flat_size = _vec.size();
		auto flat_it = _vec.insert(_vec.end(), make_flat_iterator(first), make_flat_iterator(last));
		difference_type flat_size = _vec.size() - *(*ii);

		// +1 so they point to the next value
		auto next_it = _next_vec_in_row.insert(_next_vec_in_row.end(), ii + 1, ii + flat_size + 1);
		
		auto row_num = std::distance(first, last);
		// auto fill_iter = ValueAssignIterator<IndexIterator<difference_type>>(END_ID);
		// auto row_beg = _row_to_vec.insert(where.policy().iter(), fill_iter, fill_iter + row_num);
		auto row_beg = _row_to_vec.insert(where.policy().iter(), row_num, END_ID);

		auto row_it = row_beg;
		while(first != last) {
		// for(auto it = next_it; it != _next_vec_in_row.end(); ++it) {
			// TODO: LINEAR constexpr branching
			std::advance(next_it, first->size() - 1);
			*next_it = END_ID;
			++next_it;

			if (!first->empty())
				*row_it = *ii;
			// *row_it = *ii + first->empty() * (END_ID - *ii);

			ii += first->size();
			++first;
			++row_it;
		}

		return get_row_iter(row_beg);
	}


	iterator insert(const_iterator where, size_type n, const value_type& value) {
		auto ii = IteratorIterator(IndexIterator(difference_type(_next_vec_in_row.size())));

		if (value.empty()) {
			if constexpr (LINEAR)
				return get_row_iter(_row_to_vec.insert(where.policy().iter(), n, END_ID));
			else
				return get_row_iter(_row_to_vec.insert(where.policy().iter(), ii, ii + n));
		}

		typename ranges_vec_type::const_iterator row_it =
			_row_to_vec.insert(where.policy().iter(), ii, ii + n);

		auto flat_it = _vec.insert(_vec.end(), value.begin(), value.end());

		// +1 so they point to the next value
		_next_vec_in_row.insert(_next_vec_in_row.end(), ii + 1, ii + value.size() + 1);

		if constexpr (LINEAR)
			_next_vec_in_row.back() = END_ID;
		else
			_next_vec_in_row.back() = *(_next_vec_in_row.end() - value.size() - 1);
	
		return get_row_iter(row_it);
	}



};



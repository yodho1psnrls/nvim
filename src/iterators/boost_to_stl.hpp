#pragma once

#include <iterator>
#include <boost/iterator/iterator_categories.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/find.hpp>


// Define the mapping between Boost iterator category tags and std::iterator_category


// Define the map of Boost iterator category tags to std::iterator_category types
using iterator_category_map = boost::mpl::map<
    boost::mpl::pair<boost::iterators::random_access_traversal_tag, std::random_access_iterator_tag>,
    boost::mpl::pair<boost::iterators::bidirectional_traversal_tag, std::bidirectional_iterator_tag>,
    boost::mpl::pair<boost::iterators::forward_traversal_tag, std::forward_iterator_tag>
    // boost::mpl::pair<boost::iterators::input_traversal_tag, std::input_iterator_tag>,
    // boost::mpl::pair<boost::iterators::output_traversal_tag, std::output_iterator_tag>
>;


// Metafunction to get the std::iterator_category from a Boost iterator category tag
template <typename BoostTag>
struct to_std_iterator_category {
    using type = typename boost::mpl::at<iterator_category_map, BoostTag>::type;
};

template <typename BoostTag>
using to_std_iterator_category_t = typename to_std_iterator_category<BoostTag>::type;



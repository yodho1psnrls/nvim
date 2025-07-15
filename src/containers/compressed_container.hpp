#pragma once

// NOTE: Given any nested structure of mult_containers, property_containers
// , vector of tuples and so on, it creates a container with the nested structure
// internally, but its public interface is as a mult container compressed
// into a single liner level
// Example:
// CompressedContainer<std::vector<std::tuple<els0...> MultContainer<els1...>>
// will have the public interface of MultContainer<els0..., els1...>



# For hpp only library
add_library(iterators INTERFACE)

# Specify include directories
target_include_directories(iterators INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(iterators INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



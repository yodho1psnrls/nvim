
# For hpp only library
add_library(graph INTERFACE)

# Specify include directories
target_include_directories(graph INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(graph INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



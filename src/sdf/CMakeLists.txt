# For hpp only library
add_library(sdf INTERFACE)

# Specify include directories
target_include_directories(sdf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(sdf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


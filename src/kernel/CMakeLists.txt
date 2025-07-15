
# For hpp only library
add_library(kernel INTERFACE)

# Specify include directories
target_include_directories(kernel INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(kernel INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



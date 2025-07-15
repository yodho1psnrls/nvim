
# For hpp only library
add_library(matrices INTERFACE)

# Specify include directories
target_include_directories(matrices INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(matrices INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



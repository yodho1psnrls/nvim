
# For hpp only library
add_library(mesh INTERFACE)

# Specify include directories
target_include_directories(mesh INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(mesh INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



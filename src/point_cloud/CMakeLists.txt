
# For hpp only library
add_library(point_cloud INTERFACE)

# Specify include directories
target_include_directories(point_cloud INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(point_cloud INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



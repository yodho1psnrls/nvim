
# For hpp only library
add_library(engines INTERFACE)

# Specify include directories
target_include_directories(engines INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(engines INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



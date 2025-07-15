
# For hpp only library
add_library(containers INTERFACE)

# Specify include directories
target_include_directories(containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


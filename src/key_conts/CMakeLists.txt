
# For hpp only library
add_library(key_conts INTERFACE)

# Specify include directories
target_include_directories(key_conts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Good for build time speed up
target_precompile_headers(key_conts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# add_subdirectory(adaptors)

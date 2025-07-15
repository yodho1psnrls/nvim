# This for example allows you to use:
# #ifdef TEST_BUILD
#   // Code or behavior only needed for testing
# #endif
add_definitions(-DTEST_BUILD)


# Collect all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Collect all .h and .hpp files in the current directory
file(GLOB HEADERS "*.h" "*.hpp")

# Add Google Test
find_package(GTest REQUIRED)

# Add the executable
add_executable(proj_test ${SOURCES} ${HEADERS})
#target_link_libraries(test_target proj_lib)
target_link_libraries(proj_test PRIVATE proj_lib gtest gtest_main)


# Link the src library/executable and test libraries
# Its awready linked, because it is installed with msys2 where
# also is the compiler and its libraries
#target_link_libraries(test_target PRIVATE gtest gtest_main main_exe)



# Include directories for tests
#target_include_directories(test_target PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})



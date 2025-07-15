# Create the main executable with .cpp files in the src directory
#add_executable(proj
#  main.cpp
  # Add other .cpp files
#)

# Collect all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")
# Remove main.cpp from the list of sources
list(FILTER SOURCES EXCLUDE REGEX "main.cpp")
# Now, SOURCES will contain all .cpp files except main.cpp

# Collect all .h and .hpp files in the current directory
file(GLOB HEADERS "*.h" "*.hpp")

add_library(proj_lib ${SOURCES} ${HEADERS})
set_target_properties(proj_lib PROPERTIES LINKER_LANGUAGE CXX)
add_executable(proj main.cpp)
target_link_libraries(proj proj_lib)

#add_executable(proj ${SOURCES} ${HEADERS})


add_subdirectory(matrices)
add_subdirectory(key_matrices)
add_subdirectory(iterators)
add_subdirectory(containers)

add_subdirectory(kernel)
add_subdirectory(mesh)
add_subdirectory(graph)
add_subdirectory(point_cloud)

add_subdirectory(sdf)
add_subdirectory(engines)



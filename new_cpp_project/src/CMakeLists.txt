# Create the main executable with .cpp files in the src directory
#add_executable(proj
#  main.cpp
  # Add other .cpp files
#)

# Collect all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Collect all .h and .hpp files in the current directory
file(GLOB HEADERS "*.h" "*.hpp")

# Add the executable
add_executable(proj ${SOURCES} ${HEADERS})

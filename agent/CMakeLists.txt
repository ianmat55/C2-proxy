cmake_minimum_required(VERSION 3.15)
project(agent)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include 
)

# Add source files
file(GLOB SOURCES "src/*.cpp")  # Update the path to source files

# Add the executable
add_executable(agent ${SOURCES} ${HEADERS})  # Add ${HEADERS} to the target


cmake_minimum_required(VERSION 3.10)
project(game_algorithms CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)

# Define source files
file(GLOB_RECURSE GAME_ALGORITHMS_SOURCES "*.cpp")
list(FILTER GAME_ALGORITHMS_SOURCES EXCLUDE REGEX "src/test.cpp$")
list(FILTER GAME_ALGORITHMS_SOURCES EXCLUDE REGEX "src/pybind/.*\.cpp$")

# Define include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create shared library
add_library(game_algorithms SHARED ${GAME_ALGORITHMS_SOURCES})

# Set public include directories for targets that link against this library
target_include_directories(game_algorithms PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


# Create Python module
file(GLOB_RECURSE PYBIND_SOURCES "src/pybind/*.cpp")
pybind11_add_module(game_algorithms_py 
    ${PYBIND_SOURCES}
)

target_include_directories(game_algorithms_py PRIVATE 
    ${pybind11_INCLUDE_DIRS})
target_link_libraries(game_algorithms_py PRIVATE game_algorithms)

# Set Python module output directory
set_target_properties(game_algorithms_py PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PYTHON_LIBRARY_OUTPUT_DIRECTORY})


# Add the test executable
add_executable(test_game_algorithms src/test.cpp)
target_link_libraries(test_game_algorithms PRIVATE game_algorithms)
cmake_minimum_required(VERSION 3.10)
project(data-aquisition-system)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find Boost
find_package(Boost 1.65.1 REQUIRED COMPONENTS system)

# include directories
include_directories(${Boost_INCLUDE_DIRS} src)

# target executable and its source files
add_executable(das src/main.cpp)

# link Boost libraries to the target executable
target_link_libraries(das ${Boost_LIBRARIES})

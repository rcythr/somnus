
cmake_minimum_required(VERSION 2.8)
project(SOMNUS)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -O2") 

# Add the Executable and link in pthreads.
file(GLOB_RECURSE SOURCES "src/*.cc") 

add_library(somnus ${SOURCES})

# Include our .hpp files.
include_directories(${CMAKE_SOURCE_DIR})

# Include Boost
find_package(Boost REQUIRED coroutine context)
include_directories(${Boost_INCLUDE_DIR})

# Link Libraries
target_link_libraries(somnus ${Boost_LIBRARIES})
target_link_libraries(somnus pthread)

add_subdirectory(examples)

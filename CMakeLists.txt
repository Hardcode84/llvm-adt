
cmake_minimum_required(VERSION 3.20.0)

set(LLVM_REQUIRED_CXX_STANDARD 17)

project(llvm-adt LANGUAGES CXX C)

file(GLOB_RECURSE HEADERS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp)

add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES_LIST} ${HEADERS_LIST})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(third-party/unittest)

add_subdirectory(tests)


cmake_minimum_required(VERSION 3.20.0)

set(LLVM_REQUIRED_CXX_STANDARD 17)

project(llvm-adt LANGUAGES CXX C)

add_library(${CMAKE_PROJECT_NAME} INTERFACE)
target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(third-party/unittest)

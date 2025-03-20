
cmake_minimum_required(VERSION 3.20.0)

set(LLVM_REQUIRED_CXX_STANDARD 17)

option(LLVM_ADT_ENABLE_TESTS "Enable Tests" ON)
option(LLVM_ADT_UNITY_BUILD "Unity Build" OFF)

project(llvm-adt LANGUAGES CXX C)

file(GLOB_RECURSE HEADERS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

if (LLVM_ADT_UNITY_BUILD)
    set(SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/lib/Support.cpp)
else()
    file(GLOB_RECURSE SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp)
    list(REMOVE_ITEM SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/lib/Support.cpp)
endif()

add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES_LIST} ${HEADERS_LIST})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (LLVM_ADT_ENABLE_TESTS)
    add_subdirectory(third-party/unittest)
    add_subdirectory(tests)
endif()

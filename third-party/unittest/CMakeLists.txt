########################################################################
# Experimental CMake build script for Google Test.
#
# Consider this a prototype.  It will change drastically.  For now,
# this is only for people on the cutting edge.
#
# To run the tests for Google Test itself on Linux, use 'make test' or
# ctest.  You can select which tests to run using 'ctest -R regex'.
# For more options, run 'ctest --help'.
########################################################################
#
# Project-wide settings

if(WIN32)
  add_definitions(-DGTEST_OS_WINDOWS=1)
endif()

add_library(llvm_gtest STATIC
  googletest/src/gtest-all.cc
  googlemock/src/gmock-all.cc
)

target_link_libraries(llvm_gtest PRIVATE
    llvm-adt
)

target_include_directories(llvm_gtest
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/googletest/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/googlemock/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/llvm-gtest/>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/llvm-gmock/>
  PRIVATE googletest googlemock
)

add_subdirectory(UnitTestMain)


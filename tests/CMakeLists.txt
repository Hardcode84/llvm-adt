file(GLOB_RECURSE HEADERS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(llvm-adt-test ${SOURCES_LIST} ${HEADERS_LIST})

target_link_libraries(llvm-adt-test PRIVATE
    llvm-adt
    llvm_gtest
    llvm_gtest_main
)

add_custom_target(check-llvm-adt
    COMMAND llvm-adt-test
    DEPENDS llvm-adt-test
)

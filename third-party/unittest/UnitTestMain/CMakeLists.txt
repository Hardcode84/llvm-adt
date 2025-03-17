
add_library(llvm_gtest_main STATIC TestMain.cpp)

target_link_libraries(llvm_gtest_main PRIVATE
    llvm-adt
    llvm_gtest
)

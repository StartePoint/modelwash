add_executable(test_wash test_wash.cpp)
target_link_libraries(test_wash PRIVATE modelwash_core)
add_test(NAME test_wash COMMAND test_wash)

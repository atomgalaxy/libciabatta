cmake_minimum_required(VERSION 3.13)
enable_testing()

add_executable(test_example example.cpp)
target_link_libraries(test_example ciabatta)
target_compile_features(test_example PUBLIC cxx_std_20)

add_executable(test_chained_init example.cpp)
target_link_libraries(test_chained_init ciabatta)

add_test(NAME test_example COMMAND test_example)
add_test(NAME test_chained_init COMMAND test_chained_init)

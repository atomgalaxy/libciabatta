cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED PROJECT_NAME)
  set(CIABATTA_TESTS_NOT_SUBPROJECT ON)
endif()

project(ciabatta_tests
        LANGUAGES CXX
        VERSION 1.0.0
        DESCRIPTION "Tests for the libciabatta library."
        HOMEPAGE_URL "https://github.com/atomgalaxy/libciabatta")

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(
    FATAL_ERROR
      "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt"
    )
endif()

# This is used also as a test driver to see if the ciabatta finder works. In
# that case, it's an independent project
if(CIABATTA_TESTS_NOT_SUBPROJECT)
  find_package(ciabatta REQUIRED)
endif()

enable_testing()

add_executable(test_example example.cpp)
target_link_libraries(test_example PUBLIC ciabatta::ciabatta)
target_compile_features(test_example PUBLIC cxx_std_20)

add_executable(test_chained_init example.cpp)
target_link_libraries(test_chained_init PUBLIC ciabatta::ciabatta)

add_test(NAME test_example COMMAND test_example)
add_test(NAME test_chained_init COMMAND test_chained_init)

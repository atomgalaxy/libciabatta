cmake_minimum_required(VERSION 3.13)

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

# Is this file being run as an independent test driver for find_package or
# add_subdirectory?
if(CIABATTA_TEST_FIND_PACKAGE)
  find_package(ciabatta REQUIRED)
elseif(CIABATTA_TEST_ADD_SUBDIRECTORY)
  set(CIABATTA_TEST_ADD_SUBDIRECTORY OFF)
  set(CIABATTA_SUPPRESS_TESTS ON)
  add_subdirectory(${CIABATTA_TEST_ADD_SUBDIRECTORY_PATH})
  set(CIABATTA_SUPPRESS_TESTS OFF)
endif()

if(NOT CIABATTA_SUPPRESS_TESTS)
  enable_testing()

  add_executable(test_example example.cpp)
  target_link_libraries(test_example PUBLIC ciabatta::ciabatta)
  target_compile_features(test_example PUBLIC cxx_std_20)

  add_executable(test_chained_init test_chained_init.cpp)
  target_link_libraries(test_chained_init PUBLIC ciabatta::ciabatta)

  add_executable(test_provides test_provides.cpp)
  target_link_libraries(test_provides PUBLIC ciabatta::ciabatta)

  add_test(NAME test_example COMMAND test_example)
  add_test(NAME test_chained_init COMMAND test_chained_init)
  add_test(NAME test_provides COMMAND test_provides)
endif()

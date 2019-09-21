cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

project(
  ciabatta
  LANGUAGES CXX
  VERSION 3.0.0
  DESCRIPTION
    "The Sandwich Mixin Pattern Support Library. Header-only, and really small, most composable."
  HOMEPAGE_URL "https://github.com/atomgalaxy/libciabatta")

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(
    FATAL_ERROR
      "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt"
    )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)
include(CPack)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(ciabatta INTERFACE)
target_compile_features(ciabatta INTERFACE cxx_std_14)
target_include_directories(
  ciabatta
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(ciabatta::ciabatta ALIAS ciabatta)

add_subdirectory("test")

# deployment/installation
if(NOT_SUBPROJECT)
  set(CIABATTA_CMAKE_CONFIG_DESTINATION
      "${CMAKE_INSTALL_LIBDIR}/cmake/ciabatta/")

  configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CMake/ciabatta-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ciabatta-config.cmake
    INSTALL_DESTINATION
    ${CIABATTA_CMAKE_CONFIG_DESTINATION})

  # create and install an export set for catch target as ciabatta
  install(TARGETS ciabatta EXPORT ciabatta-targets DESTINATION
                                                   ${CMAKE_INSTALL_LIBDIR})

  install(EXPORT ciabatta-targets
          NAMESPACE ciabatta::
          DESTINATION ${CIABATTA_CMAKE_CONFIG_DESTINATION})

  # This logic shamelessly copied from Catch2. By default, FooConfigVersion is
  # tied to architecture that it was generated on. Because ciabatta is header-
  # only, it is arch-independent and thus ciabatta-configVersion should not be
  # tied to the architecture it was generated on.
  #
  # CMake does not provide a direct customization point for this in
  # `write_basic_package_version_file`, but it can be accomplished indirectly by
  # temporarily redefining `CMAKE_SIZEOF_VOID_P` to an empty string. Note that
  # just undefining the variable could be insufficient in cases where the
  # variable was already in CMake cache
  set(CIABATTA_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
  set(CMAKE_SIZEOF_VOID_P "")
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ciabatta-config-version.cmake"
    COMPATIBILITY SameMajorVersion)
  set(CMAKE_SIZEOF_VOID_P ${CIABATTA_CMAKE_SIZEOF_VOID_P})

  install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ciabatta-config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/ciabatta-config-version.cmake"
          DESTINATION ${CIABATTA_CMAKE_CONFIG_DESTINATION})

  # Provide some pkg-config integration
  set(PKGCONFIG_INSTALL_DIR
      "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig"
      CACHE PATH "Path where ciabatta.pc is installed")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/ciabatta.pc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/ciabatta.pc @ONLY)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ciabatta.pc"
          DESTINATION ${PKGCONFIG_INSTALL_DIR})

  set(CPACK_PACKAGE_CONTACT "https://github.com/atomgalaxy/libciabatta")

endif(NOT_SUBPROJECT)

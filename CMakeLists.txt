cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

project(
  ciabatta
  VERSION 1.0.0
  DESCRIPTION
    "The Sandwich Mixin Pattern Support Library. Header-only, and really small, most composable."
  HOMEPAGE_URL "https://github.com/atomgalaxy/libciabatta")

add_library(ciabatta INTERFACE)
set_target_properties(ciabatta PROPERTIES INTERFACE_COMPILE_FEATURES cxx_std_14)
target_include_directories(ciabatta INTERFACE include/)

add_subdirectory("test")

if(NOT_SUBPROJECT)
  set(CIABATTA_CMAKE_CONFIG_DESTINATION
      "${CMAKE_INSTALL_LIBDIR}/cmake/ciabatta")
  configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CMake/ciabatta-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ciabatta-config.cmake
    INSTALL_DESTINATION
    ${CIABATTA_CMAKE_CONFIG_DESTINATION})

  # create and install an export set for catch target as ciabatta::Catch
  install(TARGETS ciabatta EXPORT ciabattaTargets DESTINATION
                                                  ${CMAKE_INSTALL_LIBDIR})

  install(EXPORT ciabattaTargets
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
    "${CMAKE_CURRENT_BINARY_DIR}/ciabatta-configVersion.cmake"
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

  include(CPack)

endif(NOT_SUBPROJECt)

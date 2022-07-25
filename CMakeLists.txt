cmake_minimum_required(VERSION 3.0)
project(napf VERSION 0.0.0 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif()

# options
option(BUILD_EXAMPLES "build examples" ON)
option(BUILD_FORTRAN_MODULE "build fortran module" ON)
option(SPLINEPY_EXT "add splinepy extension" ON)

# config
set(exe_dest "bin")
set(incl_dest "include")
set(lib_dest "lib")
set(cfg_dest "${lib_dest}/cmake/${PROJECT_NAME}")
set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(version_config "${gen_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${gen_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# sources
set(CXX_HEADERS src/napf.hpp third_party/nanoflann.hpp)

# Interface Library, since it's header only lib
add_library(napf INTERFACE)
add_library(napf::napf ALIAS napf)
target_include_directories(napf
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party>
        $<INSTALL_INTERFACE:${incl_dest}>)
set_target_properties(napf PROPERTIES CXX_STANDARD 11)
#target_compile_options(napf PRIVATE -O3) # everyone can enjoy optimization

if(BUILD_FORTRAN_MODULE)
  message("*** building additional fortran module ***")
  message("*** ------ NOT IMPLEMENTED ----------- ***")
endif(BUILD_FORTRAN_MODULE)

if(BUILD_EXAMPLES)
  message("*** building examples ***")
  message("*** ------ NOT IMPLEMENTED ----------- ***")
endif(BUILD_EXAMPLES)

if(SPLINEPY_EXT)
  target_compile_definitions(napf INTERFACE -DSPLINEPYEXT)
endif(SPLINEPY_EXT)

# configure config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    "cmake/config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${cfg_dest}"
)

install(
    TARGETS napf
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION ${lib_dest}
    ARCHIVE DESTINATION ${lib_dest}
    INCLUDES DESTINATION "${incl_dest}"
)
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${cfg_dest}"
)

install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${cfg_dest}"
)
install(
    FILES ${CXX_HEADERS}
    DESTINATION ${incl_dest}
)

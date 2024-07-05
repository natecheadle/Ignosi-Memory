cmake_minimum_required(VERSION 3.21)

find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS lockfree)

set(CMAKE_CXX_STANDARD 20)

add_library(
  ${PROJECT_NAME} "dll_allocate.cpp" "detail/dll_object_pool_data.cpp"
                  "dll_object_pool.cpp")

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(BUILD_SHARED_LIBS AND WIN32)
  target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE "EXPORT_SPEC=__declspec(dllexport)"
    INTERFACE "EXPORT_SPEC=__declspec(dllimport)")
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC "EXPORT_SPEC=")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt Boost::lockfree)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${EXPORT_TARGET_NAME}-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES "dll_allocate.h" "dll_object_pool.h" "dll_allocator.hpp"
              "dll_unique_ptr.hpp" "dll_object_pool.hpp"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ignosi)

install(
  EXPORT ${EXPORT_TARGET_NAME}-targets
  FILE ${EXPORT_TARGET_NAME}-targets.cmake
  NAMESPACE ${EXPORT_TARGET_NAME}::
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${EXPORT_TARGET_NAME})

include(CMakePackageConfigHelpers)

set(version 1.3.0)

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${version})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 1)
set_property(TARGET ${PROJECT_NAME}
             PROPERTY INTERFACE_${EXPORT_TARGET_NAME}_MAJOR_VERSION 1)
set_property(
  TARGET ${PROJECT_NAME}
  APPEND
  PROPERTY COMPATIBLE_INTERFACE_STRING ${EXPORT_TARGET_NAME}_MAJOR_VERSION)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_TARGET_NAME}-config-version.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_TARGET_NAME}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${EXPORT_TARGET_NAME})

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_TARGET_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_TARGET_NAME}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${EXPORT_TARGET_NAME})

export(
  EXPORT ${EXPORT_TARGET_NAME}-targets
  FILE "${CMAKE_INSTALL_DATADIR}/${EXPORT_TARGET_NAME}-targets.cmake"
  NAMESPACE ${EXPORT_TARGET_NAME}::)

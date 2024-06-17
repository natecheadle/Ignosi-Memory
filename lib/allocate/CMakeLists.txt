cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocate)

find_path(IGNOSI_DLL_INCLUDE_DIRS "dll_defines.h")
set(CMAKE_CXX_STANDARD 20)

add_library(
  ${PROJECT_NAME} SHARED "dll_allocate.cpp" "detail/dll_object_pool_data.cpp"
                         "dll_object_pool.cpp")
target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
         $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${IGNOSI_DLL_INCLUDE_DIRS}>)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${EXPORT_TARGET_NAME}Targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES "dll_allocate.h" "dll_object_pool.h"
        DESTINATION include/${EXPORT_TARGET_NAME})

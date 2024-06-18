cmake_minimum_required(VERSION 3.21)

enable_language(CXX)
include(GNUInstallDirs)

project(ignosi-memory)

set(EXPORT_TARGET_NAME ${PROJECT_NAME})

if(NOT DEFINED BUILD_TESTS)
  set(BUILD_TESTS TRUE)
endif()

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json)

enable_testing()

add_compile_definitions(PUBLIC $<$<CONFIG:Debug>:DEBUG>)

add_subdirectory("lib")

if(BUILD_TESTS)
  add_subdirectory("test")
endif()

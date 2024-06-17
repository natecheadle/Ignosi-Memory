cmake_minimum_required(VERSION 3.21)

project(ignosi-memory)

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

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION share/ignosi-memory)

install(
  EXPORT ignosi-memoryTargets
  FILE ignosi-memoryTargets.cmake
  DESTINATION lib/cmake/ignosi-memory)

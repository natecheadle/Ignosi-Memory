cmake_minimum_required(VERSION 3.21)

project(my-project)

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json)

enable_testing()

add_compile_definitions(PUBLIC $<$<CONFIG:Debug>:DEBUG>)

add_subdirectory("lib")
add_subdirectory("test")

cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocate)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} SHARED "dll_allocate.cpp")
target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ignosi-dll)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)

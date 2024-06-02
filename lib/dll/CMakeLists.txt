cmake_minimum_required(VERSION 3.21)

project(ignosi-dll)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} SHARED "dll_allocate.cpp")
target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC DLL_LIB_MAJOR_VERSION=1 DLL_LIB_MINOR_VERSION=0
                         DLL_LIB_REVISION_VERSION=0 DLL_LIB_BUILD_VERSION=0)

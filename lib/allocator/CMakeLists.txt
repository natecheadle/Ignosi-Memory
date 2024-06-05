cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocator)

set(CMAKE_CXX_STANDARD 20)

find_path(IGNOSI_DLL_INCLUDE_DIRS "dll_defines.h")
add_library(${PROJECT_NAME} SHARED "pool_allocator.cpp" "pool_bucket.cpp")
target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT)

target_link_libraries(${PROJECT_NAME} PUBLIC)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}
                                                  ${IGNOSI_DLL_INCLUDE_DIRS})

target_compile_definitions(
  ${PROJECT_NAME}
  PUBLIC ALLOCATOR_LIB_MAJOR_VERSION=1 ALLOCATOR_LIB_MINOR_VERSION=0
         ALLOCATOR_LIB_REVISION_VERSION=0 ALLOCATOR_LIB_BUILD_VERSION=0)

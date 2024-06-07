cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocator)

set(CMAKE_CXX_STANDARD 20)

find_path(IGNOSI_DLL_INCLUDE_DIRS "dll_defines.h")
add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE ignosi-memory-allocate-cpp)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}
                                                     ${IGNOSI_DLL_INCLUDE_DIRS})

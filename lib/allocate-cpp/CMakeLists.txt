cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocate-cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
  ${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${PROJECT_NAME} INTERFACE ignosi-memory-allocate)

install(FILES "dll_allocator.hpp" "dll_unique_ptr.hpp" "dll_object_pool.hpp"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocate-cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE ignosi-memory-allocate)

install(FILES dll_allocator.hpp dll_pointer.hpp DESTINATION include)

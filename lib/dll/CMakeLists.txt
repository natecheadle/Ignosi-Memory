cmake_minimum_required(VERSION 3.21)

project(ignosi-dll)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

install(FILES dll_defines.h DESTINATION include)

cmake_minimum_required(VERSION 3.21)

project(my-lib)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC "string_lib.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

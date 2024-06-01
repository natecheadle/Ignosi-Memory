cmake_minimum_required(VERSION 3.21)

project(my-test-app)

include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} "main.cpp" "string_lib_test.cpp")

find_package(GTest CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest my-lib)

gtest_add_tests(TARGET ${PROJECT_NAME})

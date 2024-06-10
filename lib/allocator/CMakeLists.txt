cmake_minimum_required(VERSION 3.21)

project(ignosi-memory-allocator)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS pool)

find_path(IGNOSI_DLL_INCLUDE_DIRS "dll_defines.h")

add_library(
  ${PROJECT_NAME} SHARED "detail/dll_object_pool_wrapper.cpp"
                         "dll_object_pool.cpp" "detail/boost_pool_allocator.h")

target_compile_definitions(${PROJECT_NAME} PRIVATE EXPORT)
target_link_libraries(${PROJECT_NAME} PUBLIC ignosi-memory-allocate Boost::pool)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}
                                                  ${IGNOSI_DLL_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)

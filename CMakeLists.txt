cmake_minimum_required(VERSION 3.10)

project(safe_laser LANGUAGES CXX)

option(BUILD_TESTS "Build unit tests" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_NAME safe_laser_lib)

add_library(${LIBRARY_NAME} OBJECT
  intersection_search_helper.cpp
  safe_checker.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(EXECUTABLE_NAME safe_laser)

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

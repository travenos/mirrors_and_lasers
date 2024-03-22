cmake_minimum_required(VERSION 3.5)

project(safe_laser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_NAME safe_laser_lib)

add_library(${LIBRARY_NAME} OBJECT
  ray_segment_intersection_finder.cpp
  safe_checker.cpp
)

set(EXECUTABLE_NAME safe_laser)

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})


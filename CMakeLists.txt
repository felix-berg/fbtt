cmake_minimum_required(VERSION 3.10)

project(fbtt)
add_library(fbtt INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
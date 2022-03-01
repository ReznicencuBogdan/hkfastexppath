cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DUNICODE -D_UNICODE)

project(ConEmuCD)

add_executable(ConEmuCD main.cpp)
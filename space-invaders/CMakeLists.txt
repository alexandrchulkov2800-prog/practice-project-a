cmake_minimum_required(VERSION 3.20)

project(SpaceInvaders LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(raylib)

add_executable(SpaceInvaders
    src/main.cpp
)

target_link_libraries(SpaceInvaders PRIVATE raylib)
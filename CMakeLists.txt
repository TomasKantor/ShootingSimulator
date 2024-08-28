cmake_minimum_required(VERSION 3.10)
project(ShootingSimulator)
add_executable(ShootingSimulator main.cpp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

target_include_directories(ShootingSimulator PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/entt"
                          )

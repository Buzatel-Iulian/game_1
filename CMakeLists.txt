cmake_minimum_required(VERSION 2.8.12)

project(GAME_1)

add_definitions("-std=c++14")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(game_1 main.cpp)
target_link_libraries(game_1 ${CONAN_LIBS})

cmake_minimum_required(VERSION 3.16)
project(minecrap)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE MC_SRCS CONFIGURE_DEPENDS src/*.cpp)
target_sources(${PROJECT_NAME} PUBLIC ${MC_SRCS})

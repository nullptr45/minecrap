cmake_minimum_required(VERSION 3.16)

project(minecrap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_executable(minecrap)

file(GLOB_RECURSE MC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
target_sources(minecrap PUBLIC ${MC_SRCS})

target_compile_options(minecrap PUBLIC -Wall -Wextra)

target_link_libraries(minecrap PRIVATE glfw glad)

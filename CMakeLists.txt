cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DIST_BUILD "Distribution build" OFF)

project(minecrap)

set(SDL_STATIC ON)

add_subdirectory(thirdparty/sdl2)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/stb)

target_link_libraries(imgui PUBLIC glad SDL2-static)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static glm glad imgui stb)

cmake_minimum_required(VERSION 3.16)
project(minecrap)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE MC_SRCS CONFIGURE_DEPENDS src/*.cpp)
target_sources(${PROJECT_NAME} PUBLIC ${MC_SRCS})

add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/stb)
add_subdirectory(thirdparty/imgui)

target_include_directories(minecrap PUBLIC src ${SDL2_INCLUDE_DIRS})
target_link_libraries(minecrap ${SDL2_LIBRARIES} glm stb imgui)

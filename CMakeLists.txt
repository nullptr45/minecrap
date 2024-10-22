cmake_minimum_required(VERSION 3.16)
project(minecrap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # LSP stuff

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "-O3")
elseif(CMAKE_BUILD_TYPE MATCHES Trace)
    set(CMAKE_CXX_FLAGS "-O3")
    add_compile_definitions(ENABLE_TRACING)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-g")
endif()

file(GLOB_RECURSE MC_SRCS src/*.cpp src/*.h)
add_executable(minecrap ${MC_SRCS})
target_precompile_headers(minecrap PRIVATE src/pch.h)

project(HttpFileServer)
set(target HttpFileServer)
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(sources
    httpfileserver.cpp)

add_library(
    ${target}
    STATIC
    ${sources})

set(executable ${target}Exec)

add_executable(
    ${executable}
    main.cpp
    )
target_link_libraries(${executable} ${target})



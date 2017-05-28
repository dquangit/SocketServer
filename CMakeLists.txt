cmake_minimum_required(VERSION 3.5)
project(SocketServer C CXX)

set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE SOURCES
        server/*.cpp
        json/*.c
        entity/*.cpp
        model/*.cpp
        )

file(GLOB_RECURSE HEADERS
        server/*.h
        json/*.h
        entity/*.h
        model/*.h
        )
set(SOURCE_FILES main.cpp ${SOURCES})
add_executable(SocketServer ${SOURCE_FILES})
target_link_libraries( ${PROJECT_NAME} ${GLOB_RECURSE} -lpthread)
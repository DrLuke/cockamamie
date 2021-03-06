cmake_minimum_required(VERSION 3.3)
project(cockamamie)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(PkgConfig REQUIRED)
find_package (OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(RapidJSON REQUIRED)


pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${RapidJSON_INCLUDE_DIRS}
        /usr/include
        /usr/include/SOIL)


set(SOURCE_FILES src/main.cpp
        src/timeline.cpp src/shader.cpp src/shader.h src/TwEventGLFW3.cpp src/TwEventGLFW3.h src/keytrack.cpp src/keytrack.h)

add_executable(cockamamie ${SOURCE_FILES})

target_link_libraries(cockamamie
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${RapidJSON_LIBRARIES}
        /usr/lib/libAntTweakBar.so
        /usr/lib/libSOIL.so)
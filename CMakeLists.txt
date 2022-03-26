cmake_minimum_required(VERSION 3.21)
project(tinyrt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Build type: ${CMAKE_BUILD_TYPE}")

add_executable(${PROJECT_NAME} src/main.cc

        src/common.h
        src/shader_program.cc
        src/shader_program.h
        src/ray.cc
        src/ray.h
        src/camera.cc
        src/camera.h
        src/material.cc
        src/material.h)

add_subdirectory(external)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# Include paths
include_directories(external/)
include_directories(external/stb)
include_directories(external/tinyobjloader)
include_directories(external/glad/include)
include_directories(external/imgui)
include_directories(external/pcg32)
include_directories(external/embree/include)

set(root_folder ${CMAKE_CURRENT_LIST_DIR})

# External Libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(embree_DIR external/embree/lib/cmake/embree-3.13.3)
find_package(embree REQUIRED)

target_link_libraries(${PROJECT_NAME} glad OpenGL dl glfw imgui pcg32 stb tinyobjloader embree)
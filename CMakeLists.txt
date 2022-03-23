cmake_minimum_required(VERSION 3.21)
project(tinyrt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(root_folder ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(external)

include_directories(external/)
include_directories(external/glad/include)
include_directories(external/imgui)
include_directories(external/pcg32)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# What does this do?
#add_dependencies(${PROJECT_NAME} OpenGL::OpenGL)
#include_directories( ${OPENGL_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} glad OpenGL dl glfw imgui pcg32)
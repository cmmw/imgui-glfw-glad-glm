include(FetchContent)
cmake_minimum_required(VERSION 3.17)

project(example)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
        GIT_TAG v4.0.0
)
FetchContent_MakeAvailable(imgui-glfw-glad-glm)

add_executable(app main.cpp)

target_link_libraries(
        app
        imgui
        glm
)

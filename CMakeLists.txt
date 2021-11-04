include(FetchContent)
cmake_minimum_required(VERSION 3.17)

project(imgui_glfw_glad)

set(CMAKE_CXX_STANDARD 17)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.5
)
FetchContent_MakeAvailable(glfw)
add_subdirectory(glad)
add_subdirectory(imgui)

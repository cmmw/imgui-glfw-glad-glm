# imgui-glfw-glad

A CMake project for [GLFW](https://github.com/glfw/glfw), [Glad](https://gen.glad.sh/) and [ImGui](https://github.com/ocornut/imgui).

This project is meant to simplify the import of the OpenGL related libraries GLFW, ImGui and Glad. GLFW is included via CMake's FetchContent and ImGui is contained as a git submodule (wrapped by simple CMake project). Glad is added manually via source code.

### Versioning

The major and minor part of the version number is analogous to GLFW, the patch version is incremented whenever the project is updated. The following list contains detailed information of the versions used in each release:

| Release   | GLFW      | ImGui     | Glad                  |
|   ---     |   ---     |   ---     |   ---                 |
| v3.3.1    | v3.3.4    | v1.83     | 2.0.0 (gl 4.5 / core) |
| v3.3.0    | v3.3.3    | v1.81     | 2.0.0 (gl 4.5 / core) |

### Example

In order to use the libraries, simply include this project with CMake's FetchContent:

```cmake
include(FetchContent)

# ...

FetchContent_Declare(
        imgui-glfw-glad
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad.git
        GIT_TAG v3.3.0
)

FetchContent_MakeAvailable(imgui-glfw-glad)

add_executable(
        app
        main.cpp
)

# It is sufficient to only link ImGui as it already contains Glad and GLFW
target_link_libraries(
        app
        imgui
)

# Otherwise link only Glad and GLFW if ImGui is not needed
target_link_libraries(
        app
        glfw
        glad
)

```

If preferred, the libraries can be built separately and included in your projects.

### License

The content of this project (excluding 3rd party libraries) is licensed under the [MIT license](https://github.com/cmmw/imgui-glfw-glad/blob/master/LICENSE.md). Please refer to the particular libraries homepage/repository for more information regarding licensing.
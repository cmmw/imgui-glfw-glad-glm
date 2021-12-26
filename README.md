# imgui-glfw-glad

A CMake project for [GLFW](https://github.com/glfw/glfw)
, [Glad](https://gen.glad.sh/), [ImGui](https://github.com/ocornut/imgui) and
[glm](https://github.com/g-truc/glm).

This project is meant to simplify the import of the OpenGL related libraries
GLFW, ImGui, Glad and glm. GLFW and glm are included via CMake's FetchContent
whereas ImGui is imported as a git submodule (wrapped by simple CMake project).
Glad is added manually via source code.

### Versioning

The major and minor part of the version number is analogous to GLFW, the patch
version is incremented whenever the project is updated. The following list
contains detailed information of the versions used in each release:

| Release | GLFW   | ImGui | Glad                  | glm     |
|---------|--------|-------|-----------------------|---------|
| v3.3.5  | v3.3.6 | v1.86 | 2.0.0 (gl 4.5 / core) | 0.9.9.8 |
| v3.3.4  | v3.3.6 | v1.85 | 2.0.0 (gl 4.5 / core) | 0.9.9.8 |
| v3.3.3  | v3.3.5 | v1.85 | 2.0.0 (gl 4.5 / core) | 0.9.9.8 |
| v3.3.2  | v3.3.5 | v1.85 | 2.0.0 (gl 4.5 / core) | N/A     |
| v3.3.1  | v3.3.4 | v1.83 | 2.0.0 (gl 4.5 / core) | N/A     |
| v3.3.0  | v3.3.3 | v1.81 | 2.0.0 (gl 4.5 / core) | N/A     |

### Example

In order to use the libraries, simply include this project with CMake's
FetchContent:

```cmake
include(FetchContent)

# ...

# Use static libraries
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
        GIT_TAG v3.3.4
)

FetchContent_MakeAvailable(imgui-glfw-glad-glm)

add_executable(
        app
        main.cpp
)

# It is sufficient to only link glm and ImGui since the latter one already contains Glad and GLFW
target_link_libraries(
        app
        imgui
        glm
)

# Otherwise link only Glad, GLFW and glm if ImGui is not needed
target_link_libraries(
        app
        glfw
        glad
        glm
)

```

The folder example provides a minimal, self-contained sample program which
includes this project via FetchContent and does all the necessary initialization
of the components. In order to build it, create a build folder inside the
example directory, cd into it and execute

```shell
cmake .. -G "<Generator of your choice>"
cmake --build . --target app
```

If preferred, libraries can be built separately and included in your projects.

### License

The content of this project (excluding 3rd party libraries) is licensed under
the [MIT license](https://github.com/cmmw/imgui-glfw-glad/blob/master/LICENSE.md)
. Please refer to the particular libraries homepage/repository for more
information regarding licensing.
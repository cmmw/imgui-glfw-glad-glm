# imgui-glfw-glad

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

A CMake project for [GLFW](https://github.com/glfw/glfw)
, [Glad](https://gen.glad.sh/), [ImGui](https://github.com/ocornut/imgui) and
[glm](https://github.com/g-truc/glm).

This project is meant to simplify the import of the OpenGL related libraries GLFW, ImGui, Glad and glm. ImGui, GLFW and
glm are imported via CMake's FetchContent. Additionally, a target is created for ImGui which is built with the glfw
backend and using glad's loader. The gl extensions are generated and downloaded from https://gen.glad.sh/ during
configuration. A fallback version of glad's gl extensions for gl version 4.5 core profile is included in this project
and will be used in case curl is not available.

There are a number of cache variables to customize the behaviour of the dependency import:

| Variable            | Type   | Default             | Description                                        |
|---------------------|--------|---------------------|----------------------------------------------------|
| IGG_COMPONENTS      | list   | IMGUI;GLFW;GLAD;GLM | The components to import                           |
| IGG_GLAD_DOWNLOAD   | bool   | ON                  | Download glad or use fallback version              |
| IGG_GLAD_GL_CORE    | bool   | ON                  | If gl core or compatibility profile should be used |
| IGG_GLAD_GL_VERSION | string | 4.5                 | The gl version used by the glad loader generator   |
| IGG_IMGUI_TAG       | string | master              | The git tag of the ImGui project to import         |
| IGG_GLFW_TAG        | string | master              | The git tag of the GLFW project to import          |
| IGG_GLM_TAG         | string | master              | The git tag of the glm project to import           |

## Example

In order to use the libraries, simply include this project with CMake's
FetchContent:

```cmake
include(FetchContent)

# ...

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
        GIT_TAG v4.0.0
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

The folder example provides a minimal, self-contained example program which includes this project via FetchContent and
does all the necessary initialization of the components. In order to build it, execute the following commands from
within the source folder:

```shell
cmake -S . -B build
cmake --build build --target app -j4
```

If you want to define the version of a dependency:

```shell
cmake -S. -B build -DIGG_GLFW_TAG=v3.3.8 -DIGG_GLAD_GL_VERSION=4.6
```

The list of the components to be imported has to be put in quotes and seperated by semicolons:

```shell
cmake -S . -B build -DIGG_COMPONENTS="GLM;GLFW"
```

Sometimes it's handy to enable the output of FetchContent:

```shell
cmake -S . -B build -DFETCHCONTENT_QUIET=OFF
```

## Reducing configuration time

The first time cmake is executed to generate the build system will take some time since all the components specified in
the `IGG_COMPONENTS` variable (except glad if selected otherwise) have to be downloaded. Further reconfigurations will
not download anything if not necessary but populating the projects will still take some time.

To avoid this the CMake variable `FETCHCONTENT_FULLY_DISCONNECTED`  can be set `ON`. This will prevent CMake to update
or download any content. Be careful if you use `FetchContent` anywhere else in your project and conduct the CMake
documentation to understand the implications.
Alternatively, `FETCHCONTENT_UPDATES_DISCONNECTED_<uppercaseName>` can be set per component to disabled updates. These
variables will be cached, it is sufficient to set them once after the initial generation of the build system.

Disabling updates for GLFW:

```shell
cmake -S . -B build -DFETCHCONTENT_UPDATES_DISCONNECTED_GLFW=ON
```

Disabling updates and downloads for every dependency imported with `FetchContent`:

```shell
cmake -S . -B build -DFETCHCONTENT_FULLY_DISCONNECTED=ON
```

## License

The content of this project (excluding 3rd party libraries) is licensed under
the [MIT license](https://github.com/cmmw/imgui-glfw-glad/blob/master/LICENSE.md)
. Please refer to the particular libraries homepage/repository for more
information regarding licensing.

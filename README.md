# imgui-glfw-glad-glm

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

A CMake dependency wrapper
for [GLFW](https://github.com/glfw/glfw), [Glad](https://gen.glad.sh/), [ImGui](https://github.com/ocornut/imgui), [ImPlot](https://github.com/epezent/implot),
and [GLM](https://github.com/g-truc/glm).

This project simplifies fetching and building these libraries with CMake's `FetchContent`. ImGui is built with the GLFW
platform backend and the OpenGL3 renderer backend; ImPlot publicly links ImGui. On desktop, the `imgui` target also
publicly links GLFW, making it available to consumers.

By default, Glad loader sources for the requested OpenGL version/profile are generated and downloaded
from https://gen.glad.sh/ during configuration. Set `IGG_GLAD_USE_BUNDLED=ON` to use the included OpenGL 4.5 core loader
instead. Bundled mode ignores the requested OpenGL version/profile. It is an explicit choice, not an automatic fallback
when curl is unavailable.

## Requirements

* CMake >= 3.25
* curl (for generating Glad sources)

## Configuration

Set these cache variables before making this project available through `FetchContent` or `add_subdirectory()`:

| Variable               | Type     | Default  | Description                                                                  |
|------------------------|----------|----------|------------------------------------------------------------------------------|
| `IGG_ENABLE_IMGUI`     | `bool`   | `ON`     | Create the `imgui` target; enables GLFW.                                     |
| `IGG_ENABLE_IMPLOT`    | `bool`   | `OFF`    | Create the `implot` target; automatically enables ImGui.                     |
| `IGG_ENABLE_GLFW`      | `bool`   | `ON`     | Provide the `glfw` target.                                                   |
| `IGG_ENABLE_GLAD`      | `bool`   | `ON`     | Create the `glad` target; disabled under Emscripten.                         |
| `IGG_ENABLE_GLM`       | `bool`   | `ON`     | Provide the `glm` target.                                                    |
| `IGG_GLAD_USE_BUNDLED` | `bool`   | `OFF`    | Use bundled OpenGL 4.5 core sources, ignoring the requested version/profile. |
| `IGG_GLAD_GL_VERSION`  | `string` | `4.5`    | OpenGL version to generate.                                                  |
| `IGG_GLAD_GL_PROFILE`  | `string` | `core`   | OpenGL profile: `core` or `compatibility`.                                   |
| `IGG_IMGUI_TAG`        | `string` | `master` | ImGui Git tag, branch, or commit hash.                                       |
| `IGG_IMPLOT_TAG`       | `string` | `master` | ImPlot Git tag, branch, or commit hash.                                      |
| `IGG_GLFW_TAG`         | `string` | `master` | GLFW Git tag, branch, or commit hash; unused under Emscripten.               |
| `IGG_GLM_TAG`          | `string` | `master` | GLM Git tag, branch, or commit hash.                                         |

Enabling ImGui overrides the GLFW flag to `ON`. Enabling ImPlot automatically enables ImGui.

Under Emscripten, `glfw` is an interface target that propagates `-sUSE_GLFW=3` and `-sMAX_WEBGL_VERSION=2`
to the linker. Native GLFW is not downloaded, and Glad is neither built nor linked since Emscripten provides its own
OpenGL (WebGL) implementation.

### Deprecated options

| Variable           | Type   | Default | Replacement                                                            |
|--------------------|--------|---------|------------------------------------------------------------------------|
| `IGG_COMPONENTS`   | `list` | Unset   | Use the individual `IGG_ENABLE_<COMPONENT>` flags.                     |
| `IGG_GLAD_GL_CORE` | `bool` | Unset   | Use `IGG_GLAD_GL_PROFILE=core` for `ON`, or `compatibility` for `OFF`. |

When supplied, `IGG_COMPONENTS` overrides the component flags with a case-insensitive, semicolon-separated list. Valid
names are `IMGUI`, `IMPLOT`, `GLFW`, `GLAD`, and `GLM`; an empty list disables all components. Selecting ImPlot
automatically enables ImGui.

## Usage

Include this project from your application's `CMakeLists.txt` using `FetchContent`:

```cmake
cmake_minimum_required(VERSION 3.25)
project(my_app LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
        GIT_TAG master
)

set(IGG_ENABLE_IMPLOT ON CACHE BOOL "")

FetchContent_MakeAvailable(imgui-glfw-glad-glm)

add_executable(
        app
        main.cpp
)

# ImPlot links ImGui and its dependencies already. 
target_link_libraries(
        app
        PRIVATE
        implot
        glm
)
```

If you only use ImGui, link `imgui` instead of `implot`. For an application that needs neither ImGui nor ImPlot, only
link the required dependencies:

```cmake
target_link_libraries(
        app
        PRIVATE
        glfw
        glad
        glm
)
```

Linking fewer targets does not disable dependency imports. Set the corresponding configuration options to avoid
fetching and building unused components.

### Building the included example

The `example` directory contains an application using ImGui, ImPlot, GLFW, Glad, and GLM. It fetches this project
from GitHub using `FetchContent`. From inside the example directory, configure and build it with:

```shell
cmake -S . -B build
cmake --build build --target app --parallel 4
```

The example enables ImPlot and otherwise uses the fetched project’s default options. Its source code requires all five
components. To use bundled Glad instead of generating and downloading loader sources, configure it with:

```shell
cmake -S . -B build -DIGG_GLAD_USE_BUNDLED=ON
```

### Configuring dependencies

The following CMake commands demonstrate how to configure dependencies when building your own application that consumes this
project. Run these commands from your application’s source directory, adjusting the options to its requirements.

```shell
cmake -S . -B build -DIGG_GLFW_TAG=3.3.8 -DIGG_GLAD_GL_VERSION=4.6
```

With the default component settings, disable ImGui and Glad to leave only GLFW and GLM enabled:

```shell
cmake -S . -B build -DIGG_ENABLE_IMGUI=OFF -DIGG_ENABLE_GLAD=OFF
```

To use bundled Glad:

```shell
cmake -S . -B build -DIGG_GLAD_USE_BUNDLED=ON
```

To show FetchContent download and update output:

```shell
cmake -S . -B build -DFETCHCONTENT_QUIET=OFF
```

## Reducing configuration time

Initial configuration may take some time while enabled dependencies are downloaded. Bundled Glad and Emscripten's GLFW
interface do not require source downloads through this project. Subsequent configurations reuse existing sources, but
`FetchContent` may still check for updates.

Set `FETCHCONTENT_UPDATES_DISCONNECTED=ON` to skip remote update checks while still allowing initial downloads.
Use `FETCHCONTENT_UPDATES_DISCONNECTED_<UPPERCASE_NAME>` to control an individual dependency.
Changes to dependency declarations can still require local update steps; behavior depends on the CMake version.

`FETCHCONTENT_FULLY_DISCONNECTED=ON` disables FetchContent downloads and updates and requires sources to be available
already. It is not suitable for a fresh build without prepopulated sources. These settings affect other uses of
FetchContent in the consuming project as well, please refer to the CMake documentation for a detailed explanation.

**These settings do not control the explicit curl request used to generate Glad.** To avoid that request, use bundled
Glad or retain matching cached generated sources. Bundled mode alone does not make the other dependencies available
offline.

**Disabling updates globally:**

```shell
cmake -S . -B build -DFETCHCONTENT_UPDATES_DISCONNECTED=ON
```

**Disabling updates for GLFW:**

```shell
cmake -S . -B build -DFETCHCONTENT_UPDATES_DISCONNECTED_GLFW=ON
```

**Using existing dependency sources and bundled Glad without downloads:**

```shell
cmake -S . -B build -DFETCHCONTENT_FULLY_DISCONNECTED=ON -DIGG_GLAD_USE_BUNDLED=ON
```

## License

This project's own code is licensed under the [MIT license](LICENSE.md). Third-party libraries retain their
respective licenses; see each library's repository for details.

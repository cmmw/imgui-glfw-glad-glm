cmake_minimum_required(VERSION 3.25)

include(FetchContent)
include(CMakeDependentOption)

project(imgui-glfw-glad-glm VERSION 4.1.0)

if (DEFINED IGG_COMPONENTS)
    message(DEPRECATION "IGG_COMPONENTS is deprecated, please use the appropriate IGG_ENABLE_<COMPONENT> flags instead.")

    set(valid_components "glfw;glad;glm;imgui;implot")
    foreach (component ${IGG_COMPONENTS})
        string(TOLOWER ${component} component)
        if (NOT component IN_LIST valid_components)
            string(REPLACE ";" ", " valid_components "${valid_components}")
            message(FATAL_ERROR "Invalid component: ${component}. Valid values are: ${valid_components}")
        endif ()
    endforeach ()

    set(IGG_ENABLE_GLFW OFF CACHE BOOL "" FORCE)
    set(IGG_ENABLE_GLAD OFF CACHE BOOL "" FORCE)
    set(IGG_ENABLE_GLM OFF CACHE BOOL "" FORCE)
    set(IGG_ENABLE_IMGUI OFF CACHE BOOL "" FORCE)
    set(IGG_ENABLE_IMPLOT OFF CACHE BOOL "" FORCE)

    set(IGG_ENABLE_GLFW OFF)
    set(IGG_ENABLE_GLAD OFF)
    set(IGG_ENABLE_GLM OFF)
    set(IGG_ENABLE_IMGUI OFF)
    set(IGG_ENABLE_IMPLOT OFF)

    foreach (component ${IGG_COMPONENTS})
        string(TOUPPER ${component} component_upper)
        set(IGG_ENABLE_${component_upper} ON CACHE BOOL "" FORCE)
        set(IGG_ENABLE_${component_upper} ON)
    endforeach ()
    unset(IGG_COMPONENTS CACHE)
endif ()

option(IGG_ENABLE_GLFW "Import GLFW" ON)
option(IGG_ENABLE_GLM "Import glm" ON)
option(IGG_ENABLE_IMGUI "Import ImGui" ON)
option(IGG_ENABLE_IMPLOT "Import ImPlot" OFF)
option(IGG_GLAD_USE_BUNDLED "Use bundled Glad (gl4.5/core), requested GL version/profile will be ignored." OFF)

if (IGG_ENABLE_IMPLOT)
    set(IGG_ENABLE_IMGUI ON CACHE BOOL "" FORCE)
    set(IGG_ENABLE_IMGUI ON)
endif ()

if (IGG_ENABLE_IMGUI)
    set(IGG_ENABLE_GLFW ON CACHE BOOL "" FORCE)
    set(IGG_ENABLE_GLFW ON)
endif ()

cmake_dependent_option(IGG_ENABLE_GLAD "Create target for Glad" ON "NOT EMSCRIPTEN" OFF)

set(IGG_COMPONENTS_TO_FETCH "")

##############################
#            glad            #
##############################
if (IGG_ENABLE_GLAD)
    # Glad specific options
    set(IGG_GLAD_GL_VERSION 4.5 CACHE STRING "The target gl version")
    set(IGG_GLAD_GL_PROFILE core CACHE STRING "Specifies the gl profile to be used (core or compatibility)")

    if (IGG_GLAD_USE_BUNDLED)
        set(glad_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad")
        message(STATUS "Using bundled Glad (gl4.5/core); IGG_GLAD_GL_VERSION and IGG_GLAD_GL_PROFILE are ignored.")
    else ()
        if (DEFINED IGG_GLAD_GL_CORE)
            message(DEPRECATION "IGG_GLAD_GL_CORE is deprecated, please use IGG_GLAD_GL_PROFILE instead.")
            if (IGG_GLAD_GL_CORE)
                set(IGG_GLAD_GL_PROFILE core CACHE STRING "" FORCE)
                set(IGG_GLAD_GL_PROFILE core)
            else ()
                set(IGG_GLAD_GL_PROFILE compatibility CACHE STRING "" FORCE)
                set(IGG_GLAD_GL_PROFILE compatibility)
            endif ()
            unset(IGG_GLAD_GL_CORE CACHE)
        endif ()

        set(allowed_gl_versions "1.0;1.1;1.2;1.3;1.4;1.5;2.0;2.1;3.0;3.1;3.2;3.3;4.0;4.1;4.2;4.3;4.4;4.5;4.6")
        if (NOT IGG_GLAD_GL_VERSION IN_LIST allowed_gl_versions)
            string(REPLACE ";" ", " allowed_gl_versions "${allowed_gl_versions}")
            message(FATAL_ERROR "Invalid IGG_GLAD_GL_VERSION: ${IGG_GLAD_GL_VERSION}. Valid values are: ${allowed_gl_versions}")
        endif ()

        set(allowed_gl_profiles "core;compatibility")
        if (NOT IGG_GLAD_GL_PROFILE IN_LIST allowed_gl_profiles)
            string(REPLACE ";" ", " allowed_gl_profiles "${allowed_gl_profiles}")
            message(FATAL_ERROR "Invalid IGG_GLAD_GL_PROFILE: ${IGG_GLAD_GL_PROFILE}. Valid values are: ${allowed_gl_profiles}")
        endif ()

        set(GL_VERSION_STRING "gl${IGG_GLAD_GL_VERSION}/${IGG_GLAD_GL_PROFILE}")

        if ("${glad_INSTALLED_VERSION}" STREQUAL "${GL_VERSION_STRING}" AND IS_DIRECTORY "${glad_LAST_SOURCE_DIR}")
            message(STATUS "Avoiding repeated download of glad ${GL_VERSION_STRING}")
            set(glad_SOURCE_DIR ${glad_LAST_SOURCE_DIR})
        else ()
            find_program(IGG_CURL NAMES curl curl.exe REQUIRED)

            execute_process(
                    COMMAND ${IGG_CURL} --max-time 120 -f -D - -X POST -d generator=c&api=egl%3Dnone&api=gl%3D${IGG_GLAD_GL_VERSION}&profile=gl%3D${IGG_GLAD_GL_PROFILE}&api=gles1%3Dnone&profile=gles1%3Dcommon&api=gles2%3Dnone&api=glsc2%3Dnone&api=glx%3Dnone&api=vulkan%3Dnone&api=wgl%3Dnone&options=LOADER https://gen.glad.sh/generate
                    OUTPUT_VARIABLE out
                    RESULT_VARIABLE res
                    ERROR_VARIABLE err
            )
            if (NOT res EQUAL "0")
                message(WARNING "${IGG_CURL} returned: ${res}")
                if (err)
                    message(FATAL_ERROR "Error message: ${err}")
                else ()
                    message(FATAL_ERROR "Could not download glad")
                endif ()
            endif ()

            string(REGEX MATCH "[Ll][Oo][Cc][Aa][Tt][Ii][Oo][Nn]:[ \t]*([^\r\n]+)" location "${out}")
            set(location "${CMAKE_MATCH_1}")
            string(STRIP "${location}" location)
            if (NOT location OR location STREQUAL "/")
                message(WARNING "Could not extract location from http response")
                message(FATAL_ERROR "Response: ${out}")
            endif ()

            string(REGEX REPLACE "/$" "" location "${location}")
            string(APPEND location "/glad.zip")
            if (NOT ${location} MATCHES "^http")
                string(PREPEND location "https://gen.glad.sh")
            endif ()
            message(STATUS "Downloading glad loader sources for ${GL_VERSION_STRING} from ${location}")
            FetchContent_Declare(
                    glad
                    URL ${location}
            )
            FetchContent_MakeAvailable(glad)
            set(glad_INSTALLED_VERSION ${GL_VERSION_STRING} CACHE INTERNAL "")
            set(glad_LAST_SOURCE_DIR ${glad_SOURCE_DIR} CACHE INTERNAL "")
        endif ()
    endif ()
    file(GLOB_RECURSE glad_sources ${glad_SOURCE_DIR}/*.c ${glad_SOURCE_DIR}/*.h)
    add_library(
            glad
            STATIC
            ${glad_sources}
    )
    target_link_libraries(
            glad
            PRIVATE
            ${CMAKE_DL_LIBS}
    )
    target_include_directories(
            glad
            PUBLIC
            ${glad_SOURCE_DIR}/include
    )
endif ()

if (IGG_ENABLE_GLFW)
    set(IGG_GLFW_TAG master CACHE STRING "Define the glfw git tag (e.g. master, 3.3.8, 3.3.7, ...). Default: master")
    if (EMSCRIPTEN)
        add_library(glfw INTERFACE)
        target_link_options(glfw INTERFACE -sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2)
    else ()
        FetchContent_Declare(
                glfw
                GIT_REPOSITORY https://github.com/glfw/glfw.git
                GIT_TAG ${IGG_GLFW_TAG}
        )
        list(APPEND IGG_COMPONENTS_TO_FETCH glfw)
    endif ()
endif ()

if (IGG_ENABLE_GLM)
    set(IGG_GLM_TAG master CACHE STRING "Defines the glm git tag, branch or commit-hash (e.g. master, 0.9.9.8, 0.9.9.7, ...). Default: master")
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG ${IGG_GLM_TAG}
    )
    list(APPEND IGG_COMPONENTS_TO_FETCH glm)
endif ()

if (IGG_ENABLE_IMGUI)
    set(IGG_IMGUI_TAG master CACHE STRING "Defines the imgui git tag, branch or commit-hash (e.g. master, v1.92.9, v1.92.8, ...). Default: master")
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG ${IGG_IMGUI_TAG}
    )
    list(APPEND IGG_COMPONENTS_TO_FETCH imgui)
endif ()

if (IGG_ENABLE_IMPLOT)
    set(IGG_IMPLOT_TAG master CACHE STRING "Defines the implot git tag, branch or commit-hash (e.g. master, v1.0, v0.17, ...). Default: master")
    FetchContent_Declare(
            implot
            GIT_REPOSITORY https://github.com/epezent/implot.git
            GIT_TAG ${IGG_IMPLOT_TAG}
    )
    list(APPEND IGG_COMPONENTS_TO_FETCH implot)
endif ()

foreach (component ${IGG_COMPONENTS_TO_FETCH})
    FetchContent_MakeAvailable(${component})
endforeach ()


if (IGG_ENABLE_IMGUI)
    file(GLOB imgui_SRC ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/*.h)
    add_library(
            imgui
            STATIC
            ${imgui_SRC}
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
    )

    target_link_libraries(
            imgui
            PUBLIC
            glfw
    )

    if (NOT EMSCRIPTEN)
        target_link_libraries(
                imgui
                PRIVATE
                ${CMAKE_DL_LIBS}
        )
    endif ()

    target_include_directories(
            imgui
            PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
    )
    if (IGG_ENABLE_IMPLOT)
        file(GLOB implot_SRC ${implot_SOURCE_DIR}/*.cpp ${implot_SOURCE_DIR}/*.h)
        add_library(
                implot
                STATIC
                ${implot_SRC}
        )

        target_include_directories(
                implot
                PUBLIC
                ${implot_SOURCE_DIR}
        )

        target_link_libraries(
                implot
                PUBLIC
                imgui
        )
    endif ()
endif ()


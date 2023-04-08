cmake_minimum_required(VERSION 3.25)

include(FetchContent)

project(imgui-glfw-glad-glm VERSION 4.0.0)

set(CMAKE_CXX_STANDARD 17)

set(IGG_COMPONENTS "IMGUI;GLFW;GLAD;GLM" CACHE STRING "The components to import and build")

foreach (comp IN ITEMS ${IGG_COMPONENTS})
    set(IMPORT_${comp} ON)
endforeach ()

if (IMPORT_GLAD)
    option(IGG_GLAD_DOWNLOAD "If set to ON glad gl extensions will be generated and downloaded if OFF the included version (gl4.5/core) will be used" ON)
    if (IGG_GLAD_DOWNLOAD)
        set(IGG_GLAD_GL_VERSION 4.5 CACHE STRING "The gl version for which to generate extension for with glad")
        option(IGG_GLAD_GL_CORE "The gl profile to generate extensions for with glad. ON = core profile, OFF = compatibility profile" ON)
        if (IGG_GLAD_GL_CORE)
            set(GLAD_GL_PROFILE core)
        else ()
            set(GLAD_GL_PROFILE compatibility)
        endif ()
    else ()
        unset(IGG_GLAD_GL_VERSION CACHE)
        unset(IGG_GLAD_GL_CORE CACHE)
    endif ()
else ()
    unset(IGG_GLAD_DOWNLOAD CACHE)
    unset(IGG_GLAD_GL_VERSION CACHE)
    unset(IGG_GLAD_GL_CORE CACHE)
endif ()
if (IMPORT_IMGUI)
    set(IGG_IMGUI_TAG master CACHE STRING "Defines the imgui version (e.g. master, v1.89.4, v1.89.3, ...")
    set(IMPORT_GLFW ON)
else ()
    unset(IGG_IMGUI_TAG CACHE)
endif ()
if (IMPORT_GLFW)
    set(IGG_GLFW_TAG master CACHE STRING "Defines the glfw version (e.g. master, 3.3.8, 3.3.7, ...) ")
else ()
    unset(IGG_GLFW_TAG CACHE)
endif ()
if (IMPORT_GLM)
    set(IGG_GLM_TAG master CACHE STRING "Defines the glm version (e.g. master, 0.9.9.8, 0.9.9.7, ...")
else ()
    unset(IGG_GLM_TAG CACHE)
endif ()

##############################
#            glad            #
##############################
if (IMPORT_GLAD)
    if (NOT IGG_GLAD_DOWNLOAD)
        message(STATUS "Using included version of glad loader (gl 4.5/core) ")
        set(glad_SOURCE_DIR glad)
    else ()
        if ("${glad_INSTALLED_VERSION}" STREQUAL "${IGG_GLAD_GL_VERSION}-${GLAD_GL_PROFILE}")
            message(STATUS "Avoiding repeated download of glad gl ${IGG_GLAD_GL_VERSION}/${GLAD_GL_PROFILE}")
            set(glad_SOURCE_DIR ${glad_LAST_SOURCE_DIR})
        else ()
            find_program(IGG_CURL NAMES curl curl.exe)
            if (NOT IGG_CURL)
                message(STATUS "Could NOT find curl, using included version of glad loader (gl 4.5/core)")
                set(glad_SOURCE_DIR glad)
            else ()
                execute_process(COMMAND ${IGG_CURL} -s -D - -X POST -d generator=c&api=egl%3Dnone&api=gl%3D4.5&profile=gl%3Dcore&api=gles1%3Dnone&profile=gles1%3Dcommon&api=gles2%3Dnone&api=glsc2%3Dnone&api=glx%3Dnone&api=vulkan%3Dnone&api=wgl%3Dnone&options=LOADER https://gen.glad.sh/generate OUTPUT_VARIABLE res)
                string(REGEX MATCH "Location: ([A-Za-z0-9_\\:/\\.]+)" location ${res})
                set(location "${CMAKE_MATCH_1}")
                string(REGEX REPLACE "/$" "" location ${location})
                string(APPEND location "/glad.zip")
                message(STATUS "Downloading glad extension for gl ${IGG_GLAD_GL_VERSION}/${GLAD_GL_PROFILE} from ${location} ")
                FetchContent_Declare(
                        glad
                        URL ${location}
                )
                FetchContent_MakeAvailable(glad)
                set(glad_INSTALLED_VERSION ${IGG_GLAD_GL_VERSION}-${GLAD_GL_PROFILE} CACHE INTERNAL "")
                set(glad_LAST_SOURCE_DIR ${glad_SOURCE_DIR} CACHE INTERNAL "")
            endif ()
        endif ()
    endif ()

    add_library(
            glad
            ${glad_SOURCE_DIR}/src/gl.c
            ${glad_SOURCE_DIR}/include/glad/gl.h
            ${glad_SOURCE_DIR}/include/KHR/khrplatform.h
    )
    target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/include)
endif ()

if (IMPORT_GLFW)
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${IGG_GLFW_TAG}
    )
    list(APPEND components glfw)
endif ()

if (IMPORT_GLM)
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG ${IGG_GLM_TAG}
    )
    list(APPEND components glm)
endif ()

if (IMPORT_IMGUI)
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG ${IGG_IMGUI_TAG}
    )
    list(APPEND components imgui)
endif ()

if (components)
    FetchContent_MakeAvailable(${components})
endif ()

##############################
#            imgui           #
##############################
if (IMPORT_IMGUI)
    file(GLOB imgui_SRC ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/*.h)
    add_library(
            imgui
            ${imgui_SRC}
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
    )
    target_link_libraries(imgui PUBLIC glfw glad)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")
    target_include_directories(
            imgui
            PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
    )
endif ()

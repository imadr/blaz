cmake_minimum_required(VERSION 3.12)

project(blaz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHICS_API "OpenGL")

set(COMMON_SOURCES
    src/cfgreader.cpp
    src/cfgreader.h
    src/game.cpp
    src/game.h
    src/error.h
    src/filesystem.h
    src/logger.cpp
    src/logger.h
    src/types.h
    src/platform.h
    src/my_math.h
    src/my_math.cpp
    src/color.h
    src/color.cpp
    src/my_time.h
    src/renderer.cpp
    src/renderer.h
    src/texture.cpp
    src/texture.h
    src/camera.cpp
    src/camera.h
    src/node.cpp
    src/node.h
)

if (GRAPHICS_API STREQUAL "OpenGL")
    list(APPEND COMMON_SOURCES src/opengl.h)
    list(APPEND COMMON_SOURCES src/renderer_opengl.cpp)
endif()

if (WIN32)
    set(PLATFORM_SOURCES
        src/filesystem_win32.cpp
        src/utils_win32.cpp
        src/utils_win32.h
        src/platform_win32.cpp
        src/my_time_win32.cpp
    )

    if (GRAPHICS_API STREQUAL "OpenGL")
        list(APPEND COMMON_SOURCES src/opengl_win32.cpp)
    endif()
endif()

if (UNIX AND NOT APPLE)
    set(PLATFORM_SOURCES
        src/filesystem_linux.cpp
        src/platform_linux.cpp
        src/my_time_linux.cpp
    )

    if (GRAPHICS_API STREQUAL "OpenGL")
        list(APPEND COMMON_SOURCES src/opengl_linux.cpp)
    endif()
endif()

file(GLOB SAMPLES
    samples/*/CMakeLists.txt
)

add_library(blaz
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES})

target_include_directories(blaz PUBLIC src)

if (WIN32)
    target_link_libraries(blaz gdi32 user32)

    if (GRAPHICS_API STREQUAL "OpenGL")
        target_link_libraries(blaz Opengl32.lib)
    endif()
endif()

foreach(SAMPLE ${SAMPLES})
    get_filename_component(SAMPLE_DIR ${SAMPLE} DIRECTORY)
    add_subdirectory(${SAMPLE_DIR})
endforeach()
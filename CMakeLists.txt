cmake_minimum_required(VERSION 3.12)

project(blaz)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_LINUX TRUE)
    add_compile_definitions(OS_LINUX)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_WINDOWS TRUE)
    add_compile_definitions(OS_WINDOWS)
endif()

set(MISC_TARGET_FOLDER "misc")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${MISC_TARGET_FOLDER})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHICS_APIS "OpenGL" "D3D11" "Vulkan")

if(NOT GRAPHICS_API)
    set(GRAPHICS_API "OpenGL" CACHE STRING "Graphics API" FORCE)
endif()

set_property(CACHE GRAPHICS_API PROPERTY STRINGS ${GRAPHICS_APIS})

option(DEBUG_RENDERER "Debug Renderer" ON)
option(ADDRESS_SANITIZED "Address sanitizer (kinda buggy)" OFF)

if(DEBUG_RENDERER)
    add_definitions(-D DEBUG_RENDERER)
endif()

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
    src/mesh.cpp
    src/mesh.h
    src/physics.cpp
    src/physics.h
)

if (EMSCRIPTEN)
    add_compile_definitions(EMSCRIPTEN)
    set(GRAPHICS_API "WebGL")
endif()

if (GRAPHICS_API STREQUAL "OpenGL")
    list(APPEND COMMON_SOURCES src/opengl_loader/opengl_loader.h)
    list(APPEND COMMON_SOURCES src/renderer_opengl.cpp)
elseif (GRAPHICS_API STREQUAL "WebGL")
    list(APPEND COMMON_SOURCES src/renderer_webgl.cpp)
elseif (GRAPHICS_API STREQUAL "D3D11")
    list(APPEND COMMON_SOURCES src/renderer_d3d11.cpp)
elseif (GRAPHICS_API STREQUAL "Vulkan")
    list(APPEND COMMON_SOURCES src/vulkan_loader/vulkan_header.h)
    list(APPEND COMMON_SOURCES src/vulkan_loader/vulkan_loader.h)
    list(APPEND COMMON_SOURCES src/vulkan_loader/vulkan_loader.cpp)
    list(APPEND COMMON_SOURCES src/renderer_vulkan.cpp)
endif()

if (EMSCRIPTEN)
    set(PLATFORM_SOURCES
        src/platform_wasm.cpp
        src/filesystem_wasm.cpp
        src/my_time_wasm.cpp
    )
elseif (OS_WINDOWS)
    set(PLATFORM_SOURCES
        src/filesystem_win32.cpp
        src/utils_win32.cpp
        src/utils_win32.h
        src/platform_win32.cpp
        src/my_time_win32.cpp
    )

    if (GRAPHICS_API STREQUAL "OpenGL")
        list(APPEND COMMON_SOURCES src/opengl_loader/opengl_loader_win32.cpp)
    endif()
elseif (OS_LINUX)
    set(PLATFORM_SOURCES
        src/filesystem_linux.cpp
        src/platform_linux.cpp
        src/my_time_linux.cpp
    )

    if (GRAPHICS_API STREQUAL "OpenGL")
        list(APPEND COMMON_SOURCES src/opengl_loader/opengl_loader_linux.cpp)
    endif()
endif()

file(GLOB SAMPLES
    samples/*/CMakeLists.txt
)

add_library(blaz
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES})

target_include_directories(blaz PUBLIC src)

if(ADDRESS_SANITIZED)
    target_compile_options(blaz PRIVATE -fsanitize=address)
    target_link_options(blaz PRIVATE -fsanitize=address)
endif()

option(UNITY_BUILD "Unity Build" OFF)

if(UNITY_BUILD)
    set_target_properties(blaz PROPERTIES UNITY_BUILD ON)
endif()

if (OS_WINDOWS)
    target_link_libraries(blaz gdi32 user32)

    if (GRAPHICS_API STREQUAL "OpenGL")
        target_link_libraries(blaz Opengl32.lib)
    endif()
endif()

foreach(SAMPLE ${SAMPLES})
    get_filename_component(SAMPLE_DIR ${SAMPLE} DIRECTORY)
    add_subdirectory(${SAMPLE_DIR})
endforeach()
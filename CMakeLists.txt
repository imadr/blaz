cmake_minimum_required(VERSION 3.12)

project(blaz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

if (WIN32)
    set(PLATFORM_SOURCES
        src/filesystem_win32.cpp
        src/utils_win32.cpp
        src/utils_win32.h
        src/platform_win32.cpp
        src/my_time_win32.cpp
    )
endif()

if (UNIX AND NOT APPLE)
    set(PLATFORM_SOURCES
        src/filesystem_linux.cpp
        src/platform_linux.cpp
        src/my_time_linux.cpp
    )
endif()

file(GLOB SAMPLES
    samples/*/CMakeLists.txt
)

include_directories(src)

add_library(blaz
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES})

if (WIN32)
    target_link_libraries(blaz gdi32 user32)
endif()

foreach(SAMPLE ${SAMPLES})
    get_filename_component(SAMPLE_DIR ${SAMPLE} DIRECTORY)
    add_subdirectory(${SAMPLE_DIR})
endforeach()
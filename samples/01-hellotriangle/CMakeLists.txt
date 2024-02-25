set(SAMPLE_NAME 01-hellotriangle)

file(GLOB SAMPLE_SOURCES
    *.cpp
    *.h
)

add_executable(${SAMPLE_NAME} ${SAMPLE_SOURCES})

target_link_libraries(${SAMPLE_NAME} blaz)

add_custom_target(copy_data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
add_dependencies(${SAMPLE_NAME} copy_data)
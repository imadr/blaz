file(GLOB SAMPLE_SOURCES
    *.cpp
    *.h
)

add_executable(${SAMPLE_NAME} ${SAMPLE_SOURCES})

target_link_libraries(${SAMPLE_NAME} blaz)

add_custom_target(${SAMPLE_NAME}_DATA
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
set_target_properties(${SAMPLE_NAME}_DATA PROPERTIES FOLDER ${MISC_TARGET_FOLDER})
add_dependencies(${SAMPLE_NAME} ${SAMPLE_NAME}_DATA)
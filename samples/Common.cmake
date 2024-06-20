file(GLOB SAMPLE_SOURCES
    *.cpp
    *.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${SAMPLE_NAME})
add_executable(${SAMPLE_NAME} ${SAMPLE_SOURCES})

target_link_libraries(${SAMPLE_NAME} blaz)

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1")
    set_target_properties(${SAMPLE_NAME} PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/@data")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../index.html.in"
        "${CMAKE_CURRENT_BINARY_DIR}/index.html"
    )
else()
    set_property(TARGET ${SAMPLE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")

    add_custom_target(${SAMPLE_NAME}_DATA
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/data
    )
    set_target_properties(${SAMPLE_NAME}_DATA PROPERTIES FOLDER ${MISC_TARGET_FOLDER})
    add_dependencies(${SAMPLE_NAME} ${SAMPLE_NAME}_DATA)

    add_custom_target(${SAMPLE_NAME}_INTERNAL_DATA
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../internal_shaders/" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/internal_data
    )
    set_target_properties(${SAMPLE_NAME}_INTERNAL_DATA PROPERTIES FOLDER ${MISC_TARGET_FOLDER})
    add_dependencies(${SAMPLE_NAME} ${SAMPLE_NAME}_INTERNAL_DATA)
endif()

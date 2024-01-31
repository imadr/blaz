set(SAMPLE_NAME 01-hellotriangle)

file(GLOB SAMPLE_SOURCES
    *.cpp
    *.h
)

add_executable(${SAMPLE_NAME} ${SAMPLE_SOURCES})

target_link_libraries(${SAMPLE_NAME} blaz)

file(COPY data/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
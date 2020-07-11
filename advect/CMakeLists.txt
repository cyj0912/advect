file(GLOB MY_SOURCES *.cpp *.c)
file(GLOB MY_HEADERS *.h)

find_package(Threads REQUIRED)

add_executable (advect ${MY_SOURCES} ${MY_HEADERS})
target_link_libraries(advect d3d11 D3DCompiler)
target_link_libraries(advect imgui)
target_link_libraries(advect glm)
target_link_libraries(advect Threads::Threads)

# Copy shaders
file(GLOB HLSL_SOURCES *.hlsl)
add_custom_command(TARGET advect POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${HLSL_SOURCES} $<TARGET_FILE_DIR:advect>)

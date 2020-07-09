file(GLOB MY_SOURCES *.cpp *.c)
file(GLOB MY_HEADERS *.h)

add_executable (advect ${MY_SOURCES} ${MY_HEADERS})
target_link_libraries(advect d3d11 D3DCompiler)

target_link_libraries(advect imgui)

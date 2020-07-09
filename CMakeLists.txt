# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("advect")

# Add ImGui here
file(GLOB IMGUI_SOURCES imgui/*.cpp)
set(IMGUI_IMPL_SRC imgui/examples/imgui_impl_win32.cpp imgui/examples/imgui_impl_dx11.cpp)
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_IMPL_SRC})
target_include_directories(imgui PUBLIC ./imgui)
target_include_directories(imgui PUBLIC ./imgui/examples)

# Include sub-projects.
add_subdirectory ("advect")

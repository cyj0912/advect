cmake_minimum_required (VERSION 3.17)

project(advect CUDA CXX)

if(NOT DEFINED CMAKE_CUDA_STANDARD)
  set(CMAKE_CUDA_STANDARD 14)
  set(CMAKE_CUDA_STANDARD_REQUIRED True)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
endif()

set(CMAKE_CUDA_ARCHITECTURES 35 50 72)

# Add ImGui here
file(GLOB IMGUI_SOURCES imgui/*.cpp)
set(IMGUI_IMPL_SRC imgui/examples/imgui_impl_win32.cpp imgui/examples/imgui_impl_dx11.cpp)
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_IMPL_SRC})
target_include_directories(imgui PUBLIC ./imgui)
target_include_directories(imgui PUBLIC ./imgui/examples)

# Add glm here
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ./glm)

# Include sub-projects.
add_subdirectory ("advect")

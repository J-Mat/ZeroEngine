cmake_minimum_required (VERSION 3.5)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(LearningDX12)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 使用宽字符
add_definitions(-DUNICODE)

add_definitions(-DDEBUG)

add_definitions(-DSOLUTION_DIR="${PROJECT_SOURCE_DIR}")

SET(SOLUTION_DIR {PROJECT_SOURCE_DIR})

project(LearningDX12 LANGUAGES CXX)

# Add sub directories
add_subdirectory(Source/Core)
add_subdirectory(Source/Box)
add_subdirectory(Source/Shape)
add_subdirectory(Source/Waves)
add_subdirectory(Source/LitColumns)
add_subdirectory(Source/09_Crate)
add_subdirectory(Source/09_TexColumns)
add_subdirectory(Source/09_TexWaves)
add_subdirectory(Source/10_FogWaves)
add_subdirectory(Source/11_Stencil)
add_subdirectory(Source/12_Billboard)
#　add_subdirectory(Source/13_VecAdd)
add_subdirectory(Source/13_WavesCS)
add_subdirectory(Source/13_Blur)



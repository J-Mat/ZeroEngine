cmake_minimum_required( VERSION 3.9.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
# 使用宽字符
add_definitions(-DUNICODE)

add_definitions(-DDEBUG)

project(LearningDX12 LANGUAGES CXX)

add_subdirectory( Source/Core)
add_subdirectory( Source/HelloDX12)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT HelloDX12)
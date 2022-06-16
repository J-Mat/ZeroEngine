project(09_TexWaves)

set (SRC_FILES
    FrameResource.h
    FrameResource.cpp
    Waves.h
    Waves.cpp
)


# Create the executable
add_executable(${PROJECT_NAME}  WIN32 
    TexWavesApp.cpp
    ${SRC_FILES}
)

# Link the static library from subproject1 using it's alias sub::lib1
# Link the header only library from subproject2 using it's alias sub::lib2
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    Core
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/Shaders
)

project(09_TexWaves LANGUAGES CXX)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

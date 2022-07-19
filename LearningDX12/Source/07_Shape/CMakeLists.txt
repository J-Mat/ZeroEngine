project(07_Shape)

set (SRC_FILES
    FrameResource.h
    FrameResource.cpp
)


# Create the executable
add_executable(${PROJECT_NAME}  WIN32 
    Shape.cpp
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

project(${PROJECT_NAME} LANGUAGES CXX)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

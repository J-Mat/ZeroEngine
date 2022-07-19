project(08_LitWaves)

set (SRC_FILES
    FrameResource.h
    FrameResource.cpp
    Waves.h
    Waves.cpp
)

add_executable(${PROJECT_NAME}  WIN32 
    LitWaves.cpp
    ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}
    Core
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/Shaders
)

project(${PROJECT_NAME} LANGUAGES CXX)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

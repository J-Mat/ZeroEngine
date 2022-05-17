cmake_minimum_required( VERSION 3.9.1)

set (SRC_FILES
	Source/main.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

add_executable(HelloDX12 WIN32
	${HEADER_FILES}
	${SRC_FILES}
)

MESSAGE( STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}.")
target_include_directories(HelloDX12 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Core
)

target_link_libraries(HelloDX12
	PUBLIC Core	
)
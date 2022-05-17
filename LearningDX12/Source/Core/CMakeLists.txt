cmake_minimum_required( VERSION 3.9.1)

project(Core)


file(GLOB HEADER_FILES 
	"./*.h"
)

file(GLOB SRC_FILES 
	"./*.cpp"
)

add_library(${PROJECT_NAME}, 
	STATIC
	${HEADER_FILES}
	${SRC_FILES}
)

target_link_libraries(Core
	PUBLIC d3d12.lib
	PUBLIC dxgi.lib
	PUBLIC dxguid.lib
	PUBLIC d3dcompiler.lib
)
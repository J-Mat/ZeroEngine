cmake_minimum_required( VERSION 3.9.1)

set( HEADER_FILES
	../Common/d3dx12.h
	../Common/d3dApp.h
	../Common/d3dUtil.h
	../Common/GameTimer.h
	../Common/DDSTextureLoader.h
	../Common/MathHelper.h
)

set (SRC_FILES
	../Common/d3dApp.cpp
	../Common/d3dUtil.cpp
	../Common/GameTimer.cpp
	../Common/DDSTextureLoader.cpp
	../Common/MathHelper.cpp
	Source/main.cpp
)

add_executable(HelloDX12 WIN32
	${HEADER_FILES}
	${SRC_FILES}
)

MESSAGE( STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}.")
target_include_directories(HelloDX12 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Common
)

target_link_libraries(HelloDX12
	d3d12.lib
	dxgi.lib
	dxguid.lib
)
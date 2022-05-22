# Set the project name
project (Core)

set (SRC_FILES
	d3dApp.cpp
	d3dUtil.cpp
	GameTimer.cpp
	DDSTextureLoader.cpp
	MathHelper.cpp
	GeometryGenerator.cpp
)

set( HEADER_FILES
	d3dx12.h
	d3dApp.h
	d3dUtil.h
	GameTimer.h
	DDSTextureLoader.h
	MathHelper.h
	GeometryGenerator.h
	UploadBuffer.h
)

# Add a library with the above sources
add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES}
    ${HEADER_FILES}
)


target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

target_link_libraries(Core
	PUBLIC d3d12.lib
	PUBLIC dxgi.lib
	PUBLIC dxguid.lib
	PUBLIC d3dcompiler.lib
)
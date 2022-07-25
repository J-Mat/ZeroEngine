set(ProjectName "Runtime")

set(RuntimeSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/Core.h")

source_group(TREE ${EngineRuntimeDir} FILES ${RuntimeSourceFiles})

add_library(${ProjectName} STATIC "Core.h" "Core.cpp")

target_include_directories(${ProjectName} 
    PRIVATE "${EngineSourceDir}"
    PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "hepch.h"
)

target_link_libraries(${ProjectName}
    PUBLIC spdlog
)

if (WIN32)
    target_link_libraries(${ProjectName}
		PUBLIC d3d12.lib
		PUBLIC dxgi.lib
		PUBLIC dxguid.lib
		PUBLIC d3dcompiler.lib
	)
endif()
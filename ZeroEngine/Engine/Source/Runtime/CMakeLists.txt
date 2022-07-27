set(ProjectName "Runtime")

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${EngineRuntimeDir} FILES ${HeadList} ${SrcList})

add_library(${ProjectName} STATIC ${HeadList} ${SrcList})


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
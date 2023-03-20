set(ProjectName "Runtime")


ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${EngineRuntimeDir} FILES ${HeadList} ${SrcList})

add_library(${ProjectName} STATIC ${HeadList} ${SrcList})

target_include_directories(${ProjectName} 
    PRIVATE "${EngineSourceDir}"
    PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "hepch.h"
    PRIVATE	"${EngineIntermediateDir}"
)

target_link_libraries(${ProjectName}
    PUBLIC stb_image
	PUBLIC tinyxml2
	PUBLIC assimp
	PUBLIC imgui
	PUBLIC ImGuizmo
	PUBLIC yaml-cpp
	PUBLIC D3D12MemAlloc
	PUBLIC Utils
	PUBLIC headers
)


if (WIN32)
    target_link_libraries(${ProjectName}
		PUBLIC d3d12.lib
		PUBLIC dxgi.lib
		PUBLIC dxguid.lib
		PUBLIC d3dcompiler.lib
		PUBLIC ${ThirdPartyDir}/dxc/lib/x64/dxcompiler.lib
	)
endif()

add_dependencies(Runtime ZHT)

add_custom_command(TARGET ${ProjectName} 
	POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	"${ThirdPartyDir}/dxc/bin/x64/dxcompiler.dll"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
) #拷贝到哪个目录

add_custom_command(TARGET ${ProjectName} 
	POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	"${ThirdPartyDir}/dxc/bin/x64/dxil.dll"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
) #拷贝到哪个目录

message ( STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )

add_custom_command(
	TARGET ${ProjectName} 
    PRE_BUILD 
    COMMAND cd ${EngineToolDir}
    COMMAND cd ZeroHeaderTool
    COMMAND ZeroHeaderTool.bat
    VERBATIM
)

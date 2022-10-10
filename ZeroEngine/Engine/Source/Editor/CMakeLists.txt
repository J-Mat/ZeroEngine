set(ProjectName "ZeroEngineEditor")

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${EngineEditorDir} FILES ${HeadList} ${SrcList})

add_executable(${ProjectName} WIN32 ${HeadList} ${SrcList})

target_link_libraries(${ProjectName} PRIVATE Runtime)

target_include_directories(${ProjectName} PRIVATE
	"${EngineRuntimeDir}"
)

add_custom_command(
	TARGET ${ProjectName} 
    PRE_BUILD 
    COMMAND call ${EngineEditorDir}/test.bat
    VERBATIM
)
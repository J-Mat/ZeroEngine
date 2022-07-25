set(ProjectName "ZeroEngineEditor")
set(EditorSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/ZeroEngineEditorApp.cpp")

source_group(TREE ${EngineEditorDir} FILES ${EditorSourceFiles})

set(EditorSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/ZeroEngineEditorApp.cpp")


add_executable(${ProjectName} ${EditorSourceFiles})

target_link_libraries(${ProjectName} PRIVATE Runtime)

target_include_directories(${ProjectName} PRIVATE
	"${EngineSourceDir}"
	"${EngineRuntimeDir}"
)


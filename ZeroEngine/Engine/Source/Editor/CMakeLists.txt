set(ProjectName "ZeroEngineEditor")
set(EditorSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/ZeroEngineEditorApp.cpp")

source_group(TREE ${EngineEditorDir} FILES ${EditorSourceFiles})


add_executable(${ProjectName} ${EditorSourceFiles})


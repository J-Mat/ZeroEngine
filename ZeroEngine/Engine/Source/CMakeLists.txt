set(ThirdPartyDir "${EngineSourceDir}/ThirdParty")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineUtilsDir "${EngineSourceDir}/Utils")
set(EngineEditorDir "${EngineSourceDir}/Editor")
set(ZeroBuildToolDir "${EngineSourceDir}/ZeroBuildTool")



add_subdirectory(ThirdParty)
add_subdirectory(Utils)
add_subdirectory(ZeroBuildTool)
add_subdirectory(Runtime)
add_subdirectory(Editor)


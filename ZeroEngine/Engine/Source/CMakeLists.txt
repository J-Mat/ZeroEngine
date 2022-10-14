set(ThirdPartyDir "${EngineSourceDir}/ThirdParty")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineUtilsDir "${EngineSourceDir}/Utils")
set(EngineEditorDir "${EngineSourceDir}/Editor")
set(ZeroHeaderToolDir "${EngineSourceDir}/ZeroHeaderTool")



add_subdirectory(ThirdParty)
add_subdirectory(Utils)
add_subdirectory(ZeroHeaderTool)
add_subdirectory(Runtime)
add_subdirectory(Editor)


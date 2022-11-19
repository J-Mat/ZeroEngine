set(ThirdPartyDir "${EngineSourceDir}/ThirdParty")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineUtilsDir "${EngineSourceDir}/Utils")
set(EngineEditorDir "${EngineSourceDir}/Editor")
set(ZeroHeaderToolDir "${EngineSourceDir}/ZeroHeaderTool")

configure_file(CodeReflectionLinkFile.h.in ${CMAKE_CURRENT_SOURCE_DIR}/CodeReflectionLinkFile.h @ONLY)

add_subdirectory(ThirdParty)
add_subdirectory(Utils)
add_subdirectory(ZeroHeaderTool)
add_subdirectory(ConsoleTest)
add_subdirectory(Runtime)
add_subdirectory(Editor)


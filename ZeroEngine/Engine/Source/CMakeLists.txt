set(ThirdPartyDir "${EngineSourceDir}/ThirdParty")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineEditorDir "${EngineSourceDir}/Editor")


add_subdirectory(ThirdParty)
add_subdirectory(Runtime)
add_subdirectory(Editor)

# 很丑陋 暂时没想到好方法
target_include_directories( Runtime PUBLIC ${ThirdPartyDir}/glm)
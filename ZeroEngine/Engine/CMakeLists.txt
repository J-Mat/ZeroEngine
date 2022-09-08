set(EngineRootDir "${ProjectRootDir}/Engine")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineAssetsDir "${ProjectRootDir}/Assets")
set(EngineSourceDir "${EngineRootDir}/Source")


add_subdirectory(Source)

file(COPY ${EngineAssetsDir}
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
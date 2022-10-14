set(EngineRootDir "${ProjectRootDir}/Engine")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineUtilsDir "${EngineSourceDir}/Utils")
set(EngineAssetsDir "${ProjectRootDir}/Assets")
set(EngineConfigDir "${ProjectRootDir}/Config")
set(EngineSourceDir "${EngineRootDir}/Source")
set(EngineIntermediateDir "${ProjectRootDir}/Intermediate")
set(EngineToolDir "${ProjectRootDir}/Tools")


add_subdirectory(Source)

file(COPY ${EngineAssetsDir}
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
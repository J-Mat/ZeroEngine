set(EngineRootDir "${ProjectRootDir}/Engine")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineUtilsDir "${EngineSourceDir}/Utils")
set(EngineAssetsDir "${ProjectRootDir}/Assets")
set(EngineConfigDir "${ProjectRootDir}/Config")
set(EngineSourceDir "${EngineRootDir}/Source")
set(EngineIntermediateDir "${ProjectRootDir}/Intermediate")
set(EngineToolDir "${ProjectRootDir}/Tools")
set(CodeReflectionLinkFile "${EngineSourceDir}/CodeReflectionLinkFile.h")


add_subdirectory(Source)

file(COPY ${EngineAssetsDir}
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(ConsoleTestDir "${EngineSourceDir}/ConsoleTest")

set(ProjectName "ConsoleTest")

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${ConsoleTestDir} FILES ${HeadList} ${SrcList})

add_executable(${ProjectName} ${HeadList} ${SrcList})

target_link_libraries(${ProjectName}
    PUBLIC Utils
    PUBLIC mINI
    PUBLIC tinyxml2 
    PUBLIC headers 
)

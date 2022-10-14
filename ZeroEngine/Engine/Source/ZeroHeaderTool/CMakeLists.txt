set(ProjectName "ZHT")

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${ZeroHeaderToolDir} FILES ${HeadList} ${SrcList})

add_executable(${ProjectName} ${HeadList} ${SrcList})

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set_target_properties(${ProjectName}
        PROPERTIES
        ARCHIVE_OTPUT_DIRECTORY_${OUTPUTCONFIG} "${EngineToolDir}/ZeroHeaderTool"
        LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${EngineToolDir}/ZeroHeaderTool"
        RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${EngineToolDir}/ZeroHeaderTool"
    )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

target_link_libraries(${ProjectName}
    PUBLIC Utils
    PUBLIC mINI
)

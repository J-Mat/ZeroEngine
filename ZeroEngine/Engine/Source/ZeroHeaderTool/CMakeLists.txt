set(ProjectName "ZHT")

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${ZeroHeaderToolDir} FILES ${HeadList} ${SrcList})

add_executable(${ProjectName} ${HeadList} ${SrcList})

target_link_libraries(${ProjectName}
    PUBLIC Utils
    PUBLIC mINI
)

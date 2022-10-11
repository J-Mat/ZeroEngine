set(ProjectName "Utils")

configure_file(Config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Config.h @ONLY)

ConstructSolutionDirTree( ${CMAKE_CURRENT_SOURCE_DIR} HeadList SrcList)

source_group(TREE ${EngineUtilsDir} FILES ${HeadList} ${SrcList})

add_library(${ProjectName} STATIC ${HeadList} ${SrcList})


# 很丑陋 暂时没想到好方法
target_include_directories(${ProjectName} PUBLIC ${ThirdPartyDir}/glm)

target_link_libraries(${ProjectName}
    PUBLIC spdlog
	PUBLIC crossguid
)


target_include_directories(${ProjectName} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

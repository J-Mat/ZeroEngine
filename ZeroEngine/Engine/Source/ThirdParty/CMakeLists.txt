set(ThirdPartyFolder "ThirdParty")

if(NOT TARGET spdlog)
	option(SPDLOG_BUILD_EXAMPLE "" OFF)
	option(SPDLOG_INSTALL "" OFF)
	add_subdirectory(spdlog)
	set_target_properties(spdlog PROPERTIES FOLDER ${ThirdPartyFolder})
	NoWarning(spdlog)
endif()


if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    option(BUILD_TESTING "" OFF)
    option(GLM_TEST_ENABLE "" OFF)
    add_subdirectory(glm)
    set_target_properties(glm PROPERTIES FOLDER ${ThirdPartyFolder})
    NoWarningInterface(glm)
endif()
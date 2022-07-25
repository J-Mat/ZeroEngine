set(ThirdPartyFolder "ThirdParty")

include(${CMAKE_SOURCE_DIR}/CMake/Tools.cmake)

if(NOT TARGET spdlog)
	option(SPDLOG_BUILD_EXAMPLE "" OFF)
	option(SPDLOG_INSTALL "" OFF)
	add_subdirectory(spdlog)
	set_target_properties(spdlog PROPERTIES FOLDER ${ThirdPartyFolder})
	NoWarning(spdlog)
endif()
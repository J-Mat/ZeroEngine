set(ThirdPartyFolder "ThirdParty")

if(NOT TARGET spdlog)
	option(SPDLOG_BUILD_EXAMPLE "" OFF)
	option(SPDLOG_INSTALL "" OFF)
	add_subdirectory(spdlog)
	set_target_properties(spdlog PROPERTIES FOLDER ${ThirdPartyFolder})
	NoWarning(spdlog)
endif()

if(NOT TARGET DirectXTex)
	add_subdirectory(DirectXTex)
	set_target_properties(DirectXTex PROPERTIES FOLDER ${ThirdPartyFolder})
	NoWarning(DirectXTex)
endif()

if(NOT TARGET stb_image)
	add_subdirectory(stb_image)
    set_target_properties(stb_image PROPERTIES FOLDER ${ThirdPartyFolder})
    NoWarning(stb_image)
endif()

if(NOT TARGET mINI)
	add_subdirectory(mINI)
  set_target_properties(mINI PROPERTIES FOLDER ${ThirdPartyFolder})
  NoWarning(mINI)
endif()

if(NOT TARGET imgui)
	add_subdirectory(imgui)
    set_target_properties(imgui PROPERTIES FOLDER ${ThirdPartyFolder})
    NoWarning(stb_image)
endif()

if(NOT TARGET assimp)
    OPTION( BUILD_SHARED_LIBS
      "Build package with shared libraries."
      OFF
    )
    OPTION( ASSIMP_BUILD_FRAMEWORK
      "Build package as Mac OS X Framework bundle."
      OFF
    )
    OPTION( ASSIMP_DOUBLE_PRECISION
      "Set to ON to enable double precision processing"
      OFF
    )
    OPTION( ASSIMP_OPT_BUILD_PACKAGES
      "Set to ON to generate CPack configuration files and packaging targets"
      OFF
    )
    OPTION( ASSIMP_ANDROID_JNIIOSYSTEM
      "Android JNI IOSystem support is active"
      OFF
    )
    OPTION( ASSIMP_NO_EXPORT
      "Disable Assimp's export functionality."
      OFF
    )
    OPTION( ASSIMP_BUILD_ZLIB
      "Build your own zlib"
      OFF
    )
    OPTION( ASSIMP_BUILD_ASSIMP_TOOLS
      "If the supplementary tools for Assimp are built in addition to the library."
      OFF
    )
    OPTION ( ASSIMP_BUILD_SAMPLES
      "If the official samples are built as well (needs Glut)."
      OFF
    )
    OPTION ( ASSIMP_BUILD_TESTS
      "If the test suite for Assimp is built in addition to the library."
      OFF
    )
    OPTION ( ASSIMP_COVERALLS
      "Enable this to measure test coverage."
      OFF
    )
    OPTION( ASSIMP_INSTALL
      "Disable this if you want to use assimp as a submodule."
      OFF
    )
    OPTION ( ASSIMP_ERROR_MAX
      "Enable all warnings."
      OFF
    )
    OPTION ( ASSIMP_ASAN
      "Enable AddressSanitizer."
      OFF
    )
    OPTION ( ASSIMP_UBSAN
      "Enable Undefined Behavior sanitizer."
      OFF
    )
    OPTION ( ASSIMP_BUILD_DOCS
      "Build documentation using Doxygen."
      OFF
    )
    OPTION( ASSIMP_INJECT_DEBUG_POSTFIX
      "Inject debug postfix in .a/.so/.dll lib names"
      OFF
    )

    OPTION ( ASSIMP_IGNORE_GIT_HASH
       "Don't call git to get the hash."
       OFF
    )
    add_subdirectory(assimp)
    set_target_properties(assimp PROPERTIES FOLDER ${ThirdPartyFolder})
    set_target_properties(zlibstatic PROPERTIES FOLDER ${ThirdPartyFolder})
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER ${ThirdPartyFolder})
    NoWarning(assimp)
endif()

if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    option(BUILD_TESTING "" OFF)
    option(GLM_TEST_ENABLE "" OFF)
    add_subdirectory(glm)
    set_target_properties(glm_static PROPERTIES FOLDER ${ThirdPartyFolder})
    NoWarningInterface(glm_static)
endif()


if(NOT TARGET crossguid)
  option(BUILD_STATIC_LIBS "" ON)
  option(BUILD_TESTING "" OFF)
  option(GLM_TEST_ENABLE "" OFF)
  add_subdirectory(crossguid)
  set_target_properties(crossguid PROPERTIES FOLDER ${ThirdPartyFolder})
  NoWarningInterface(crossguid)
endif()


if(NOT TARGET headers)
    add_library(headers INTERFACE)
    target_include_directories(headers INTERFACE "${ThirdPartyDir}")
    target_include_directories(headers INTERFACE "${ThirdPartyDir}/assimp/include")
    NoWarningInterface(headers)
endif()
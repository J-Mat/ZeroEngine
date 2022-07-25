macro(RUNTIME_ADD_SOURCE)
	set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES "^/.*")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                 "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(RuntimeSourceFiles ${RuntimeSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(RUNTIME_ADD_SOURCE)


set(ProjectName "Runtime")

set(RuntimeSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/ZeroEngine.h" "${CMAKE_CURRENT_SOURCE_DIR}/ZeroEngine.cpp")

source_group(TREE ${EngineRuntimeDir} FILES ${RuntimeSourceFiles})

add_library(${ProjectName} STATIC ${RuntimeSourceFiles})


add_subdirectory(Core)

target_include_directories(${ProjectName} 
    PRIVATE "${EngineSourceDir}"
    PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "hepch.h"
)

target_link_libraries(${ProjectName}
    PUBLIC spdlog
)

if (WIN32)
    target_link_libraries(${ProjectName}
		PUBLIC d3d12.lib
		PUBLIC dxgi.lib
		PUBLIC dxguid.lib
		PUBLIC d3dcompiler.lib
	)
endif()
set(IniSrc 
    "src/mini/ini.h"
    "src/mini/ini.cpp"
)

add_library(mINI STATIC ${IniSrc})
target_include_directories(mINI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")


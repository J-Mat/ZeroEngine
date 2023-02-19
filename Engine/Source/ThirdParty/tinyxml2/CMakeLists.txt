set(tinyxml2Src 
    "tinyxml2.h"
    "tinyxml2.cpp"
)

add_library(tinyxml2 STATIC ${tinyxml2Src})
target_include_directories(tinyxml2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


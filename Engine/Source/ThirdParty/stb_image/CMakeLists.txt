set(stbSrc 
    "stb_image.h"
    "stb_image.cpp"
)

add_library(stb_image STATIC ${stbSrc})
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


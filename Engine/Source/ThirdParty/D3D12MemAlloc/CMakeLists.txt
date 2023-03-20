set(D3D12MemAllocSrc 
    "D3D12MemAlloc.h"
    "D3D12MemAlloc.cpp"
)

add_library(D3D12MemAlloc STATIC ${D3D12MemAllocSrc})
target_include_directories(D3D12MemAlloc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


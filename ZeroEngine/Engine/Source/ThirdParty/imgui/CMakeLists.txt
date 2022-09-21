set(ImGuiSrc
    "./imconfig.h"
    "./imgui.h"
    "./imgui.cpp"
    "./imgui_draw.cpp"
    "./imgui_internal.h"
    "./imgui_tables.cpp"
    "./imgui_widgets.cpp"
    "./imstb_rectpack.h"
    "./imstb_textedit.h"
    "./imstb_truetype.h"
    "./imgui_demo.cpp"
)

# Still some bugs
set(ImGuiImpl
    "./backends/imgui_impl_win32.cpp"
    "./backends/imgui_impl_win32.h"
    "./backends/imgui_impl_dx12.cpp"
    "./backends/imgui_impl_dx12.h"
)

add_library(imgui STATIC ${ImGuiSrc} ${ImGuiImpl})
target_include_directories(imgui
    PUBLIC "${ThirdPartyDir}/imgui"
)

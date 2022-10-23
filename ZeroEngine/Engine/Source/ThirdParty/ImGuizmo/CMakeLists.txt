set(ImGuizmoSourceDir "./")

set(ImGuizmoSrc
    "ImGuizmo.h"
    "ImGuizmo.cpp"
)

add_library(ImGuizmo STATIC ${ImGuizmoSrc})
target_include_directories(ImGuizmo PUBLIC "${ImGuizmoSourceDir}")
target_link_libraries(ImGuizmo PUBLIC imgui)

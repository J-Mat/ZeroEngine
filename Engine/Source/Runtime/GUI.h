#pragma once

#include "Core.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <ImGuizmo.h>
#include "GUI/GuiUtility.h"
#if defined(_WIN32)
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#endif

#include "World/Object/CoreObject.h"
#include "World/Object/ClassObject.h"
#include "World/Object/VariateProperty.h"
#include "World/Object/ClassProperty.h"
#include "World/Object/ObjectGlobal.h"

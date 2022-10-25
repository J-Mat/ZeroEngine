#pragma once

#include "Core.h"
#include "Core/Framework/Application.h"
#include "Utils.h"
#include "Render/Pipeline/RenderStage.h"
#include "Render/Pipeline/RenderPipeline.h"
#include "World/World.h"
#include "World/Actor/CameraActor.h"
#include "World/Actor/MeshActor.h"
#include "Core/Base/Input.h"
#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include "imgui/imgui.h"
#include <ImGuizmo.h>
#include "GUI/GuiUtility.h"
#if defined(_WIN32)
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#endif

#include "World/Base/CoreObject.h"
#include "World/Base/ClassObject.h"
#include "World/Base/VariateProperty.h"
#include "World/Base/ClassProperty.h"
#pragma once

#include "Core.h"
#include "Core/Framework/Application.h"
#include "Utils.h"
#include "Render/Pipeline/RenderStage.h"
#include "Render/Pipeline/RenderPipeline.h"
#include "World/Construction/ObjectConstruction.h"
#include "World/World.h"
#include "World/Actor/CameraActor.h"
#include "World/Actor/MeshActor.h"
#include "Core/Base/Input.h"
#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include "imgui/imgui.h"
#if defined(_WIN32)
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#endif
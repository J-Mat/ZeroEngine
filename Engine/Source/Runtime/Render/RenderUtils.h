#pragma once

#include "Core.h"
#include "Render/RHI/RenderItem.h"
#include "World/World.h"
#include "Render/Moudule/ImageBasedLighting.h"

namespace Zero
{
	class FRenderUtils
	{
	public:
		using FRenderFunc = std::function<void(Ref<FRenderItem>)>;
		using FDispatchComputeFunc = std::function<void(Ref<FComputeItem>)>;
		static void RenderLayer(const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle, FRenderFunc&& RenderFunc);
		static void RenderLayer(const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle);
		static void DrawRenderItem(Ref<FRenderItem> RenderItem, const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle, FRenderFunc&& RenderFunc);
		static void DrawRenderItem(Ref<FRenderItem> RenderItem, const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle);
		static Ref<FImageBasedLighting> GetIBLMoudule();
		static void SetNeedRefreshIBL() { s_bNeedRefreshIBL = true; };
		static void RefreshIBL();
		static void DispatchComputeShader(const FDispatchComputeParams DispatchComputeParams, FCommandListHandle CommandListHandle, FDispatchComputeFunc&& DispatchComputeFunc);
	private:
		static bool s_bNeedRefreshIBL;
		static Ref<FImageBasedLighting> s_IBLMoudule;
	};
}

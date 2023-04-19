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
		static void RenderLayer(ERenderLayer RenderLayerType, FCommandListHandle CommandListHandle, FRenderFunc& RenderFunc);
		static void DrawRenderItem(Ref<FRenderItem> RenderItem, FCommandListHandle CommandListHandle, FRenderFunc& RenderFunc);
		static Ref<FImageBasedLighting> GetIBLMoudule();
		static void SetNeedRefreshIBL() { s_bNeedRefreshIBL = true; };
		static void RefreshIBL();
	private:
		static bool s_bNeedRefreshIBL;
		static Ref<FImageBasedLighting> s_IBLMoudule;
	};
}

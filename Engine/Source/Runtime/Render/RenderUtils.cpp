#include "RenderUtils.h"


namespace Zero
{
	Ref<FImageBasedLighting> FRenderUtils::s_IBLMoudule = nullptr;
	bool FRenderUtils::s_bNeedRefreshIBL = false;
	void FRenderUtils::RenderLayer(ERenderLayer RenderLayerType, FCommandListHandle CommandListHandle, FRenderFunc& RenderFunc)
	{
		auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RenderLayerType);
		for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
		{
			DrawRenderItem(RenderItem, CommandListHandle, RenderFunc);
		}
	}

	void FRenderUtils::DrawRenderItem(Ref<FRenderItem> RenderItem, FCommandListHandle CommandListHandle, FRenderFunc& RenderFunc)
	{
		RenderItem->PreRender(CommandListHandle);
		RenderFunc(RenderItem);
		RenderItem->Render(CommandListHandle);
	}

	Ref<FImageBasedLighting> FRenderUtils::GetIBLMoudule()
	{
		if (s_IBLMoudule == nullptr)
		{
			Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
			Ref<FRenderItem> Items[5];
			for (int i = 0; i < 5; ++i)
			{
				Items[i] = RenderItemPool->Request();
			}
			s_IBLMoudule = CreateRef<FImageBasedLighting>(RenderItemPool->Request(), Items);
		}
		return s_IBLMoudule;
	}

	void FRenderUtils::RefreshIBL()
	{
		if (!s_bNeedRefreshIBL)
		{
			return;
		}
		GetIBLMoudule()->PreCaculate();
		s_bNeedRefreshIBL = false;
	}

}

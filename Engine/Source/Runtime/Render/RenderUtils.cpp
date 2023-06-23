#include "RenderUtils.h"


namespace Zero
{
	Ref<FImageBasedLighting> FRenderUtils::s_IBLMoudule = nullptr;
	bool FRenderUtils::s_bNeedRefreshIBL = false;
	void FRenderUtils::RenderLayer(const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle, FRenderFunc&& RenderFunc)
	{
		auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RenderSettings.RenderLayer);
		for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
		{
			DrawRenderItem(RenderItem, RenderSettings, CommandListHandle, std::forward<FRenderFunc>( RenderFunc));
		}
	}

	void FRenderUtils::RenderLayer(const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle)
	{
		auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RenderSettings.RenderLayer);
		for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
		{
			DrawRenderItem(RenderItem, RenderSettings, CommandListHandle);
		}
	}

	void FRenderUtils::DrawRenderItem(Ref<FRenderItem> RenderItem, const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle, FRenderFunc&& RenderFunc)
	{
		if (RenderItem->CanRender(CommandListHandle, RenderSettings))
		{
			RenderFunc(RenderItem);
			RenderItem->Render(CommandListHandle);
		}
	}

	void FRenderUtils::DrawRenderItem(Ref<FRenderItem> RenderItem, const FRenderParams& RenderSettings, FCommandListHandle CommandListHandle)
	{
		if (RenderItem->CanRender(CommandListHandle, RenderSettings))
		{ 
			RenderItem->Render(CommandListHandle);
		}
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

	void FRenderUtils::DispatchComputeShader(const FDispatchComputeParams DispatchComputeParams, FCommandListHandle CommandListHandle, FDispatchComputeFunc&& DispatchComputeFunc)
	{
		Ref<FComputeRenderItemPool> ComputeRenderItemPool = UWorld::GetCurrentWorld()->GetComputeRenderItemPool();
		Ref<FComputeItem> Item = ComputeRenderItemPool->Request();
		Item->SetPsoID(DispatchComputeParams.PsoID);
		Item->PreDispatch(CommandListHandle);
		DispatchComputeFunc(Item);
		Item->Dispatch(CommandListHandle, DispatchComputeParams.ThreadNumX, DispatchComputeParams.ThreadNumY, DispatchComputeParams.ThreadNumZ);
	}
}

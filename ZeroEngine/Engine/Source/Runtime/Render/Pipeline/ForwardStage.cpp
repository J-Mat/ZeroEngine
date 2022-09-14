#include "ForwardStage.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/RenderTarget.h"
#include "World/World.h"

namespace Zero
{
	void FForwardStage::OnAttach()
	{
	}

	void FForwardStage::OnDetach()
	{
	}

	void FForwardStage::OnDraw()
	{
		Ref<IDevice> Device = UWorld::GetCurrentWorld()->GetDevice();
		m_RenderTarget =  Device->GetSwapChain()->GetRenderTarget();
		m_RenderTarget->ClearBuffer();
		m_RenderTarget->Bind();
		FRenderItemPool& RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool();
		for (Ref<FRenderItem> RenderItem : RenderItemPool)
		{
			RenderItem->m_Material->SetPass();
			RenderItem->m_Material->OnDrawCall();
			RenderItem->OnDrawCall();

		}
		m_RenderTarget->UnBind();
	}
}

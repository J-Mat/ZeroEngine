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
		Ref<IDevice> Device = UWorld::GetCurrentWorld()->GetDevice();
		FRenderTargetDesc Desc;
		auto [Width, Height] = Device->GetSwapChain()->GetSize();
		Desc.Width = 900;
		Desc.Height = 600;
		m_RenderTarget = FRenderer::GraphicFactroy->CreateRenderTarget(Desc);
		TLibrary<FRenderTarget>::Push(FORWARD_STAGE, m_RenderTarget);
	}

	void FForwardStage::OnDetach()
	{
	}

	void FForwardStage::OnDraw()
	{
		//Ref<IDevice> Device = UWorld::GetCurrentWorld()->GetDevice();
		//m_RenderTarget = Device->GetSwapChain()->GetRenderTarget();
		m_RenderTarget->ClearBuffer();
		m_RenderTarget->Bind();
		FRenderItemPool& RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool();
		for (Ref<FRenderItem> RenderItem : RenderItemPool)
		{
			RenderItem->m_Material->Tick();
			RenderItem->m_Material->SetPass();
			RenderItem->m_Material->OnDrawCall();
			RenderItem->OnDrawCall();
		}
		m_RenderTarget->UnBind();
	}
}

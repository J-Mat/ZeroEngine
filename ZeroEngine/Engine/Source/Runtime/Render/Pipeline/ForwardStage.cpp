#include "ForwardStage.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/RenderTarget.h"
#include "World/World.h"
#include "World/LightManager.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	void FForwardStage::OnAttach()
	{
		Ref<IDevice> Device = UWorld::GetCurrentWorld()->GetDevice();
		FRenderTargetDesc Desc;
		Desc.Format = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::None, //   For Picking
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
		};
		m_RenderTarget = FRenderer::GraphicFactroy->CreateRenderTarget(Desc);
		TLibrary<FRenderTarget>::Push(RENDER_STAGE_FORWARD, m_RenderTarget);
	}

	void FForwardStage::OnDetach()
	{
	}

	void FForwardStage::OnDraw()
	{
		m_RenderTarget->ClearBuffer();
		m_RenderTarget->Bind();

		
		if (false)
		{
			static auto SkyboxPSO = TLibrary<FPipelineStateObject>::Fetch(PSO_SKYBOX);
			SkyboxPSO->Bind();
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_SKYBOX);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_Material->Tick();
				RenderItem->m_Material->SetPass();
				RenderItem->m_Material->OnDrawCall();
				RenderItem->OnDrawCall();
			}
		}

		{
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_PipelineStateObject->Bind();
				RenderItem->m_Material->Tick();
				RenderItem->m_Material->SetPass();
				RenderItem->m_Material->OnDrawCall();
				RenderItem->OnDrawCall();
			}
		}
		m_RenderTarget->UnBind();
	}
}

#include "ForwardStage.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
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
		m_RenderTarget->Bind();
		
		FRenderItemPool& RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool();
		for (Ref<FRenderItem> RenderItem : RenderItemPool)
		{
			RenderItem->m_Material->SetPass();
		}
		m_RenderTarget->UnBind();
	}
}

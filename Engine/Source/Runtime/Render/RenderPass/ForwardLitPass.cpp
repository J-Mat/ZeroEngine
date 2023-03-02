#include "ForwardLitPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/RenderTarget.h"
#include "World/World.h"
#include "World/LightManager.h"
#include "Render/RenderConfig.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/RHI/Texture.h"

namespace Zero
{
	FForwardLitPass::FForwardLitPass(uint32_t Width, uint32_t Height)
		: m_Width(Width),
		m_Height(Height)
	{
	}

	void FForwardLitPass::AddPass(FRenderGraph& RenderGraph)
	{
		RenderGraph.AddPass<void>("ForwardLit Pass",
			[=](FRenderGraphBuilder& Builder)
			{
				{
					FRGTextureDesc DepthDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::R32_TYPELESS
					};
					Builder.DeclareTexture(RG_RES_NAME(DepthStencil), DepthDesc);
					Builder.WriteDepthStencil(RG_RES_NAME(DepthStencil), ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc ColorDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture(RG_RES_NAME(GBufferColor), ColorDesc);
					Builder.WriteRenderTarget(RG_RES_NAME(GBufferColor), ERGLoadStoreAccessOp::Clear_Preserve);
				}

				Builder.SetViewport(m_Width, m_Height);
			},
			[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				static auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
				for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
				{
					RenderItem->PreRender(CommandListHandle);
					const auto& PSODesc = RenderItem->m_PipelineStateObject->GetPSODescriptor();
					const std::string& ShaderName = PSODesc.Shader->GetDesc().FileName;
					RenderItem->Render(CommandListHandle);
				}
			},
			ERenderPassType::Graphics,
			ERGPassFlags::None
		);
	}

	void FForwardLitPass::OnResize(uint32_t Width, uint32_t Height)
	{
	}
}

#include "DAGRenderer.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RendererAPI.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"
#include "Render/RenderUtils.h"
#include "World/LightManager.h"


namespace Zero
{
	FDAGRender::FDAGRender(uint32_t Width, uint32_t Height)
		: m_Width(Width), 
		m_Height(Height),
		m_ForwardLitPass(Width, Height),
		m_TestPostprocessPass(Width, Height),
		m_ShadowPass(1024, 1024)
	{
		CreateStaticResource();
		CreateSizeDependentResources();
	}

	FDAGRender::~FDAGRender()
	{
	}

	void FDAGRender::SetupShadow()
	{
	}

	void FDAGRender::CreateSizeDependentResources()
	{
		FTextureDesc TextureDesc =
		{
			.Width = m_Width,
			.Height = m_Height,
			.ResourceBindFlags = EResourceBindFlag::RenderTarget | EResourceBindFlag::ShaderResource,
			.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
			.Format = EResourceFormat::R8G8B8A8_UNORM,
		};
		m_FinalTexture.reset(FGraphic::GetDevice()->CreateTexture2D("FinalTexture", TextureDesc, false));
		m_FinalTexture->RegistGuiShaderResource();

		
		m_DirectLightShadowMapDebugs.resize(FLightManager::Get().GetMaxDirectLightsNum());
		FTextureDesc ShadowMapDesc = {
			.Width = m_Width,
			.Height = m_Height,
			.ResourceBindFlags = EResourceBindFlag::RenderTarget | EResourceBindFlag::ShaderResource,
			.ClearValue = FTextureClearValue(1.0f, 0),
			.Format = EResourceFormat::R8G8B8A8_UNORM
		};
		for (uint32_t LightIndex = 0; LightIndex < FLightManager::Get().GetMaxDirectLightsNum(); ++LightIndex)
		{
			std::string Name = "DirectLightShadowMapDebug" + std::to_string(LightIndex);
			m_DirectLightShadowMapDebugs[LightIndex].reset(FGraphic::GetDevice()->CreateTexture2D(Name, ShadowMapDesc, false));
			m_DirectLightShadowMapDebugs[LightIndex]->RegistGuiShaderResource();
		}
	}

	void FDAGRender::CreateStaticResource()
	{
		FBufferDesc Desc{
			.Size = sizeof(float) * 32,
			.ResourceUsage = EResourceUsage::Readback,
			.ResourceBindFlag = EResourceBindFlag::None,
			.MiscFlag = EBufferMiscFlag::BufferRaw,
			.Stride = sizeof(float),
			.Format = EResourceFormat::R32_FLOAT
		};
		m_TestReadbackBuffer.reset(FGraphic::GetDevice()->CreateBuffer(Desc));

	}

	void FDAGRender::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
		CreateSizeDependentResources();
		m_ForwardLitPass.OnResize(Width, Height);
		m_TestPostprocessPass.OnResize(Width, Height);
	}

	void FDAGRender::OnUpdate()
	{
		SetupShadow();
	}

	void FDAGRender::SetupEnvironmentMap(FRenderGraph& Rg)
	{
		static auto* Settings = FSettingManager::Get().GetSceneSettings();
		if (Settings->m_bUseSkyBox)
		{
			FRenderUtils::RefreshIBL();
			m_SkyboxPass.AddPass(Rg);
		}
		m_ShadowPass.AddPass(Rg);
	}

	void FDAGRender::ImportResource(FRenderGraph& RenderGraph)
	{
		RenderGraph.ImportTexture2D(RGResourceName::FinalTexture, m_FinalTexture.get());

		for (uint32_t LightIndex = 0; LightIndex < FLightManager::Get().GetMaxDirectLightsNum(); ++LightIndex)
		{
			std::string Name = "DirectLightShadowMapDebug" + std::to_string(LightIndex);
			RenderGraph.ImportTexture2D(RGResourceName::ShadowMaps_Debug[LightIndex], m_DirectLightShadowMapDebugs[LightIndex].get());
		}

		RenderGraph.ImportBuffer(RGResourceName::ReadBackBuffer, m_TestReadbackBuffer.get());
	}


	void FDAGRender::OnDraw()
	{
		FRenderGraph RenderGraph(m_RGResourcePool);
		ImportResource(RenderGraph);
		

		SetupEnvironmentMap(RenderGraph);

		m_ForwardLitPass.AddPass(RenderGraph);
		//m_TestPostprocessPass.AddPass(RenderGraph)
		m_PostprocessCollection.AddPasses(RenderGraph);

		m_CopyToFinalTexturePass.AddPass(RenderGraph, m_PostprocessCollection.GetFinalRsourceName());
	
		RenderGraph.Build();
		RenderGraph.Execute();
	}

	void FDAGRender::OnEndFrame()
	{
	}

}


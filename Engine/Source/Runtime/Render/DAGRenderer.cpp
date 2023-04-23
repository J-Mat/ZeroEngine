#include "DAGRenderer.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RendererAPI.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"
#include "Render/RenderUtils.h"


namespace Zero
{
	FDAGRender::FDAGRender(uint32_t Width, uint32_t Height)
		: m_Width(Width), 
		m_Height(Height),
		m_ForwardLitPass(Width, Height),
		m_ShadowPass(2048, 2048)
	{
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

	}

	void FDAGRender::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
		CreateSizeDependentResources();
		m_ForwardLitPass.OnResize(Width, Height);
	}

	void FDAGRender::OnUpdate()
	{
		SetupShadow();
	}

	void FDAGRender::SetupEnvironmentMap(FRenderGraph& Rg)
	{
		static auto* Settings = FSettingManager::Get().FecthSettings<USceneSettings>(USceneSettings::StaticGetObjectName());
		if (Settings->m_bUseSkyBox)
		{
			FRenderUtils::RefreshIBL();
			m_SkyboxPass.AddPass(Rg);
		}
	}

	void FDAGRender::OnDraw()
	{
		FRenderGraph RenderGraph(m_RGResourcePool);
		RenderGraph.ImportTexture(RGResourceName::FinalTexture, m_FinalTexture.get());

		
		m_ShadowPass.AddPass(RenderGraph);
		m_ForwardLitPass.AddPass(RenderGraph);

		SetupEnvironmentMap(RenderGraph);

		m_CopyToFinalTexturePass.AddPass(RenderGraph);
		
	
		RenderGraph.Build();
		RenderGraph.Execute();
	}
}


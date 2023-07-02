#include "PostprocessCollection.h"
#include "Data/Settings/SceneSettings.h"
#include "Data/Settings/SettingsManager.h"
#include "TaaPass.h"

namespace Zero
{
	FPostprocessCollection::FPostprocessCollection(uint32_t Width, uint32_t Height, USceneSettings* SceneSettings)
		: m_Width(Width),
		m_Height(Height),
		m_TaaPass(Width, Height)
	{
	}

	void FPostprocessCollection::OnResize(uint32_t Width, uint32_t Height)
	{
		m_TaaPass.OnResize(Width, Height);
	}

	void FPostprocessCollection::AddPasses(FRenderGraph& RenderGraph)
	{
		m_FinalResource = RGResourceName::GBufferColor;
		static auto* Settings = FSettingManager::Get().GetSceneSettings();
		if (Settings->m_AntiAliasing == EAntiAliasing::TAA)
		{
			m_FinalResource =  m_TaaPass.AddPass(RenderGraph);
		}
	}
}


#pragma once
#include "Core.h"
#include "Render/RenderGraph/RenderGraphResourceName.h"

namespace Zero
{
	class FRenderGraph;
	class USceneSettings;
	class FTaaPass;
	class FPostprocessCollection
	{
	public:
		FPostprocessCollection(uint32_t Width, uint32_t Height, USceneSettings* SceneSettings);
		void OnResize(uint32_t Width, uint32_t Height);
		void AddPasses(FRenderGraph& RenderGraph);
		FRGResourceName GetFinalRsourceName() { return m_FinalResource; }
	private:
		FRGResourceName m_FinalResource;
		uint32_t m_Width;
		uint32_t m_Height;
	private:
		FTaaPass m_TaaPass;
	};
}
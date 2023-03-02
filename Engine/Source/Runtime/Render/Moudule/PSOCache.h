#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FPSOCache : public IPublicSingleton<FPSOCache>
	{
	public:
		void RegisterDefaultPSO();
		void RegisterSkyboxPSO();
		void RegisterIBLPSO();
		void RegisterShadowPSO();
		void RegisterComputeShader();
	public:
		Ref<FPipelineStateObject> Fetch(EPipelineState PsoState);
	private:
		std::map<EPipelineState, Ref<FPipelineStateObject>> m_PsoCache{};
	};
}
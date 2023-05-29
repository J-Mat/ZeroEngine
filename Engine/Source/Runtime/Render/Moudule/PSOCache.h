#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RenderConfig.h"
#include "Delegate.h"

namespace Zero
{
	DEFINITION_MULTICAST_DELEGATE(FPsoRecreateEvent, void, uint32_t)

	class FPSOCache : public IPublicSingleton<FPSOCache>
	{
	public:
		FPSOCache();
		void RegisterErrorPSO();
		void RegisterDefaultPSO();
		void RegisterSkyboxPSO();
		void RegsiterForwardLitPSO();
		void RegisterIBLPSO();
		void RegisterShadowPSO();
		void RegisterComputeShader();
		void OnReCreateGraphicPSO(Ref<FShader> Shader);
	public:
		FPsoRecreateEvent& GetPsoRecreateEvent() { return m_PsoRecreateEvent; };
		Ref<FGraphicPipelineStateObject> FetchGraphicPso(uint32_t PsoID);
		Ref<FComputePipelineStateObject> FetchComputePso(uint32_t PsoID);
	private:
		std::vector<Ref<FGraphicPipelineStateObject>> m_GraphicPsoCache;
		std::vector<Ref<FComputePipelineStateObject>> m_ComputePsoCache;

		FPsoRecreateEvent m_PsoRecreateEvent;
	};
}
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
		void RegisterGlobalPSO();
		void RegisterDefaultPSO();
		void RegisterSkyboxPSO();
		void RegisterIBLPSO();
		void RegisterShadowPSO();
		void RegisterComputeShader();
		void OnReCreatePso(Ref<FShader> Shader);
	public:
		FPsoRecreateEvent& GetPsoRecreateEvent() { return m_PsoRecreateEvent; };
		FPipelineStateObject* Fetch(uint32_t PsoID);
	private:
		std::vector<Scope<FPipelineStateObject>> m_PsoCache;

		FPsoRecreateEvent m_PsoRecreateEvent;
	};
}
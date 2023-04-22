#pragma once
#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/RenderTarget.h"

namespace Zero
{
	class FRenderItem;
	class FImageBasedLighting
	{
	public:
		FImageBasedLighting(Ref<FRenderItem> IrradianceMapRenderItem, Ref<FRenderItem> PrefilterMapRenderItem[5]);
		void PreCaculate();
		Ref<FRenderTargetCube> GetIrradianceRTCube() { return m_IBLIrradianceMapRTCube; }
		Ref<FRenderTargetCube> GetPrefilterEnvMapRTCube() { return m_PrefilterEnvMapRTCube; }
	private:
		void InitIrradianceMap();
		void InitPrefilerMap();
		void CreateIBLIrradianceMap();
		void CreateIBLPrefilterEnvMap();	
	private:
		FCommandListHandle m_CommandListHandle;
		Ref<FRenderItem> m_IrradianceMapRenderItem = nullptr;
		Ref<FRenderItem> m_PrefilterMapRenderItems[5];
		Ref<FRenderTargetCube> m_IBLIrradianceMapRTCube = nullptr;
		Ref<FRenderTargetCube> m_PrefilterEnvMapRTCube = nullptr;
		const uint32_t m_Mips = 5;
	};
}
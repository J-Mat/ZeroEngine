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
		const std::vector<Ref<FTextureCubemap>>& GetPrefilterEnvMapTextureCubes() {return m_PrefilterEnvMapTextureCubes; }
		const Ref<FTextureCubemap> GetPrefilterEnvMapTextureCubeIndex(int32_t Index) {return m_PrefilterEnvMapTextureCubes[Index]; }
	private:
		void InitIrradianceMap();
		void InitPrefilerMap();
		void CreateIBLIrradianceMap();
		void CreateIBLPrefilterEnvMap();	
	private:
		FCommandListHandle m_CommandListHandle;
		Ref<FRenderItem> m_IrradianceMapRenderItem = nullptr;
		Ref<FRenderItem> m_PrefilterMapRenderItem[5];
		Ref<FRenderTargetCube> m_IBLIrradianceMapRTCube = nullptr;
		std::vector<Ref<FRenderTargetCube>> m_PrefilterEnvMapRTCubes;
		std::vector<Ref<FTextureCubemap>> m_PrefilterEnvMapTextureCubes;
		const int32_t m_Mips = 5;
	};
}
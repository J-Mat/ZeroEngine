#pragma once

#include "Component.h"
#include "Render/RHI/Mesh.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/ShaderBinder.h"


namespace Zero
{
	class FPerObjectConstantsBufferPool : public IPublicSingleton<FPerObjectConstantsBufferPool>
	{
	public:
		Ref<IShaderConstantsBuffer> GetPerObjectConstantsBuffer(uint32_t DeviceIndex = 0);
		void PushToPool(Ref<IShaderConstantsBuffer> Buffer);
	private:
		std::queue<Ref<IShaderConstantsBuffer>> m_IdleConstantsBuffer;
	};

	class FMaterial;
	class UMeshRenderComponent : public UComponent
	{
	public:
		UMeshRenderComponent();
		virtual ~UMeshRenderComponent();

		std::vector<Ref<FMaterial>>& GetPassMaterials(const std::string& PassName);
	private:
		Ref<FMesh> m_Mesh;
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
		std::unordered_map<std::string, std::vector<Ref<FMaterial>>> Materials;
	};
}

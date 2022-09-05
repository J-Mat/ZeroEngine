#pragma once

#include "Render/RendererAPI.h"
#include "Component.h"


namespace Zero
{
	class FMaterial;
	class UMeshRenderComponent : public UComponent
	{
	public:
		friend class UMeshActor;
		UMeshRenderComponent();
		virtual ~UMeshRenderComponent();

		std::vector<Ref<FMaterial>>& GetPassMaterials(const EMeshRenderLayerType& LayerType);
		void SetSubmeshNum(uint32_t Num) { m_SubmeshNum = Num; }
	private:
		std::unordered_map<EMeshRenderLayerType, std::vector<Ref<FMaterial>>> m_Materials;
		uint32_t m_SubmeshNum = 0;
	};
}

#pragma once

#include "Render/RendererAPI.h"
#include "Component.h"
#include "MeshRenderComponent.reflection.h"


namespace Zero
{
	class FMaterial;
	UCLASS()
	class UMeshRenderComponent : public UComponent
	{
		GENERATED_BODY()
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

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

		void SetShader(const std::string& ShaderFile) { m_ShaderFile = ShaderFile; }
		std::vector<Ref<FMaterial>>& GetPassMaterials(const EMeshRenderLayerType& LayerType);
		void SetSubmeshNum(uint32_t Num) { m_SubmeshNum = Num; }
	private:
		std::unordered_map<EMeshRenderLayerType, std::vector<Ref<FMaterial>>> m_Materials;
		uint32_t m_SubmeshNum = 0;
		
		std::string m_ShaderFile = "ForwardLit.hlsl";
		
		UPROPERTY()
		FMaterialConfig m_MaterialConfig;
	};
}

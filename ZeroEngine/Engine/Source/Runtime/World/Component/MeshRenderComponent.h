#pragma once

#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Component.h"
#include "MeshRenderComponent.reflection.h"


namespace Zero
{
	UENUM()
	enum EPsoType
	{
		PT_ForwardLit,
		PT_Fresnel,
	};

	class FMaterial;
	class FPipelineStateObject;
	UCLASS()
	class UMeshRenderComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		friend class UMeshActor;
		friend class USkyActor;
		UMeshRenderComponent();
		virtual ~UMeshRenderComponent();

		void virtual PostInit() override;
		void SetEnableMaterial(bool bEnable);
		std::vector<Ref<FMaterial>>& GetPassMaterials(uint32_t LayerLayer);
		void SetSubmeshNum(uint32_t Num) {	m_SubmeshNum = Num; }
		void AttachRenderLayer(uint32_t RenderLayer){m_RenderLayer |= RenderLayer;}

		Ref<FPipelineStateObject> GetPipelineStateObject() { return m_PipelineStateObject;}
		void SetPipelineStateObject(Ref<FPipelineStateObject> PipelineStateObject) { m_PipelineStateObject; }

		virtual void PostEdit(UProperty* Property) override;
		void AttachParameters();
		void SwitchPso();
	private:
		std::unordered_map<uint32_t, std::vector<Ref<FMaterial>>> m_Materials;
		uint32_t m_SubmeshNum = 0;
		
		bool m_bEnableMaterial = true;
		
		UPROPERTY()
		FMaterialHandle m_MaterialHandle;

		UPROPERTY(Invisible)
		uint32_t m_RenderLayer = 0;

		UPROPERTY()
		EPsoType m_Psotype = EPsoType::PT_ForwardLit;
	private:
		Ref<FPipelineStateObject> m_PipelineStateObject = nullptr;
	};
}

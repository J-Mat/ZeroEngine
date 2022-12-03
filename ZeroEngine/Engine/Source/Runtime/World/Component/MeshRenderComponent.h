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
		UPROPERTY(Invisible)
		PT_Skybox,

		UPROPERTY(Invisible)
		PT_Light,

		UPROPERTY()
		PT_ForwardLit,

		UPROPERTY()
		PT_Fresnel,

		UPROPERTY()
		PT_NDF,

		UPROPERTY()
		PT_GGX
	};

	class FMaterial;
	class FPipelineStateObject;
	UCLASS()
	class UMeshRenderComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		friend class UMeshActor;
		UMeshRenderComponent();
		virtual ~UMeshRenderComponent();

		void virtual PostInit() override;
		void SetEnableMaterial(bool bEnable);
		std::vector<Ref<FMaterial>>& GetPassMaterials(uint32_t LayerLayer);
		void SetSubmeshNum(uint32_t Num) {	m_SubmeshNum = Num; }
		void AttachRenderLayer(int32_t RenderLayer){m_RenderLayer |= RenderLayer;}

		Ref<FPipelineStateObject> GetPipelineStateObject();
		void SetPsoType(EPsoType PosType);

		virtual void PostEdit(UProperty* Property) override;
		void AttachParameters();
		void SwitchPso();
	private:
		std::unordered_map<uint32_t, std::vector<Ref<FMaterial>>> m_Materials;
		uint32_t m_SubmeshNum = 0;
		
		UPROPERTY(Invisible)
		bool m_bEnableMaterial = true;
		
		UPROPERTY()
		FMaterialHandle m_MaterialHandle;

		UPROPERTY(Invisible)
		int32_t m_RenderLayer = 0;

		UPROPERTY()
		EPsoType m_Psotype = EPsoType::PT_ForwardLit;

		UPROPERTY()
		std::map<std::string, ZMath::FColor3> m_ColorParameter;

		UPROPERTY()
		std::map<std::string, FFloatSlider> m_FloatSliderParameter;

	private:
		Ref<FPipelineStateObject> m_PipelineStateObject = nullptr;
	};
}

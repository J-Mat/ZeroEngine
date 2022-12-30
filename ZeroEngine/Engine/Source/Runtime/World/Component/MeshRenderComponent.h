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
		PT_DirectLight,

		UPROPERTY(Invisible)
		PT_PointLight,

		UPROPERTY()
		PT_ForwardLit,
	};

	UENUM()
	enum EShadingMode
	{
		UPROPERTY()
		SM_Light,

		UPROPERTY()
		SM_Test,
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
		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr);

		Ref<FPipelineStateObject> GetPipelineStateObject();
		void SetPsoType(EPsoType PosType);

		void SetShadingMode(EShadingMode ShadingMode);

		virtual void PostEdit(UProperty* Property) override;
		void AttachPso();
		void AttachParameters();
		void UpdateSettings();
		void SwitchPso();
	private:
		Ref<IShaderConstantsBuffer> m_PerObjConstantsBuffer = nullptr;
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
		EShadingMode m_ShadingMode = EShadingMode::SM_Light;

	public:
		UPROPERTY(Format = hlsl)
		FShaderFileHandle m_ShaderFile = "Shader\\ForwardLit.hlsl";

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, FTextureHandle> m_Textures;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, FFloatSlider> m_Floats;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, ZMath::FColor3> m_Colors;

	private:
		Ref<FPipelineStateObject> m_PipelineStateObject = nullptr;
	};
}

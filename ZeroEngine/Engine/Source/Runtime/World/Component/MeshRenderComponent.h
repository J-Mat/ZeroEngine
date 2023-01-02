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
	
	class FPipelineStateObject;
	class FMaterial;
	struct FRenderLayerInfo
	{
		Ref<FPipelineStateObject> PipelineStateObject = nullptr;
		std::vector<Ref<FMaterial>> Materials;
	};

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
		void AttachRenderLayer(int32_t RenderLayer);
		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, uint32_t RenderLayer = RENDERLAYER_OPAQUE);

		Ref<FPipelineStateObject> GetPipelineStateObject(uint32_t RenderLayer);
		void SetPsoType(EPsoType PosType, uint32_t RenderLayer = RENDERLAYER_OPAQUE);

		void SetShadingMode(EShadingMode ShadingMode);

		virtual void PostEdit(UProperty* Property) override;
		void AttachParameters();
		void UpdateSettings();
	private:
		Ref<IShaderConstantsBuffer> m_PerObjConstantsBuffer = nullptr;
		std::unordered_map<uint32_t, FRenderLayerInfo> m_LayerInfo;
		uint32_t m_SubmeshNum = 0;
		
		UPROPERTY(Invisible)
		bool m_bEnableMaterial = true;
		
		UPROPERTY()
		FMaterialHandle m_MaterialHandle;

		UPROPERTY(Invisible)
		int32_t m_RenderLayer = 0;


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

#pragma once

#include "Render/RenderConfig.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Component.h"
#include "MeshRenderComponent.reflection.h"


namespace Zero
{
	UENUM()
	enum EShadingMode
	{
		UPROPERTY()
		SM_Light,

		UPROPERTY()
		SM_Test,
	};
	
	class FMaterial;
	struct FRenderLayerInfo
	{
		uint32_t PsoID = EPsoID::InvalidPso;
		std::vector<Ref<FMaterial>> Materials;
	};

	UCLASS()
	class UMeshRenderComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		UMeshRenderComponent();
		virtual ~UMeshRenderComponent();

		void virtual PostInit() override;
		void SetEnableMaterial(bool bEnable);
		std::vector<Ref<FMaterial>>& GetPassMaterials(ERenderLayer LayerLayer);
		void SetSubmeshNum(uint32_t Num);
		void AttachRenderLayer(ERenderLayer RenderLayer, uint32_t PsoID);
		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, ERenderLayer RenderLayer = ERenderLayer::Opaque);

		uint32_t GetPsoID(ERenderLayer RenderLayer);
		void  OnRecreatePso(uint32_t PsoID);

		void SetShadingMode(EShadingMode ShadingMode);

		int32_t GetRenderLayer() { return m_RenderLayer;}
		virtual void PostEdit(UProperty* Property) override;
		void AttachParametersToShader();
		void UpdateSettings();
		Ref<IShaderConstantsBuffer> GetAndPreDraCallPerObjConstanBuffer();
		const std::unordered_map<ERenderLayer, Ref<FRenderLayerInfo>>& GetLayerInfo() { return m_LayerInfo; }
	private:
		Ref<IShaderConstantsBuffer> m_PerObjConstantsBuffer = nullptr;
		std::unordered_map<ERenderLayer, Ref<FRenderLayerInfo>> m_LayerInfo;
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
	};
}

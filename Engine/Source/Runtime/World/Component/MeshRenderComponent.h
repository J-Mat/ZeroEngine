#pragma once

#include "Render/RenderConfig.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
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
	
	class FShaderParamsSettings;
	struct FRenderLayerInfo
	{
		uint32_t PsoID = EGraphicPsoID::GlobalPso;
		std::vector<Ref<FShaderParamsSettings>> ShaderParamsSettings;
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
		std::vector<Ref<FShaderParamsSettings>>& GetPassMaterials(ERenderLayer LayerLayer);
		void SetSubmeshNum(uint32_t Num);
		void AttachRenderLayer(ERenderLayer RenderLayer, uint32_t PsoID = ZERO_INVALID_ID);
		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, ERenderLayer RenderLayer = ERenderLayer::Opaque);

		uint32_t GetPsoID(ERenderLayer RenderLayer);
		void  OnReCreateGraphicPSO(uint32_t PsoID);

		void SetShadingMode(EShadingMode ShadingMode);

		int32_t GetRenderLayer() { return m_RenderLayer;}
		virtual void PostEdit(UProperty* Property) override;
		virtual void Tick() override;
		void AttachParametersToShader();
		void UpdateSettings();
		Ref<IShaderConstantsBuffer> GetAndPreDraCallPerObjConstanBuffer();
		const std::unordered_map<ERenderLayer, Ref<FRenderLayerInfo>>& GetLayerInfo() { return m_LayerInfo; }

	private:
		bool m_bUpdateIfDirty = false;
		std::map<std::string, Ref<FTexture2D>> m_Textures;
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
		std::map<std::string, FTextureHandle> m_TextureHandles;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, FFloatSlider> m_Floats;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, ZMath::FColor3> m_Colors;
	};
}

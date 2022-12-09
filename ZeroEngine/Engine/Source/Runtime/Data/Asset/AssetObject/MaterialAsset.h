#pragma once

#include "Core.h"
#include "Asset.h"
#include "World/Base/CoreObject.h"
#include "MaterialAsset.reflection.h"

namespace Zero
{
	UCLASS()
	class UMaterialAsset : public UAsset
	{
		GENERATED_BODY()
	public:
		UMaterialAsset() = default;
		virtual const char* GetAssetExtention() override { return "mat"; };

		virtual void PostEdit(UProperty* Property) override;

	public:
		UPROPERTY(Format = hlsl)
		FShaderFileHandle m_ShaderFile = "Shader/ForwardLit.hlsl";

		UPROPERTY()
		std::map<std::string, FTextureHandle> m_Textures;

		UPROPERTY()
		std::map<std::string, FFloatSlider> m_Floats;
	};
}
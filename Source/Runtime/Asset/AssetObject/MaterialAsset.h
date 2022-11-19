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

	public:
		UPROPERTY(Format = hlsl)
		FAssignedFile m_ShaderFile = "Shader/ForwardLit.hlsl";

		UPROPERTY()
		std::map<std::string, FTextureHandle> m_Textures;
		
		UPROPERTY()
		FFloatSlider m_Sider;

		UPROPERTY()
		ZMath::FColor4 m_Color;
	};
}
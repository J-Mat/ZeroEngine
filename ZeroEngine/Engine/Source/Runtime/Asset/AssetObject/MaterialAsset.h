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
		FTextureHandle m_Diffuse = "default";
	};
}
#pragma once

#include "Core.h"
#include "World/Base/CoreObject.h"
#include "Asset.reflection.h"

namespace Zero
{	
	UCLASS()
	class UAsset : public UCoreObject
	{
		GENERATED_BODY()
	public:
		UAsset() = default;
		const FAssetHandle& GetAssetHandle() const { return m_AssetHandle; }
		void SetAssetName(const std::string AssetName) { m_AssetName = AssetName; };
		std::string GetAssetName() { return m_AssetName; };
		void SetAssetHandle(FAssetHandle& Handle) { m_AssetHandle = Handle; }
		virtual const char* GetAssetExtention() { return "unknown"; };
	private: 
		UPROPERTY(Invisible)
		FAssetHandle m_AssetHandle;

		UPROPERTY(Disable)
		std::string m_AssetName;
		
		UPROPERTY()
		FFloatSlider m_FloatSlider;
	};

}	
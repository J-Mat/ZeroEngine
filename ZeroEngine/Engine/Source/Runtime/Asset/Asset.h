#pragma once

#include "Core.h"
#include "SerializeUtility.h"
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
		virtual const char* GetAssetExtention() { return "unknown"; };
		virtual void Serialize();
		virtual	bool Deserialize();
	private: 
		UPROPERTY()	
		std::filesystem::path m_Path;
	};
}	
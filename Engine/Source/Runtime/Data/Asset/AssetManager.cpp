#include "AssetManager.h"
#include "AssetManager.h"
#include "Utils/StringUtils.h"
#include "World/Object/ObjectGlobal.h"
#include "ZConfig.h"
#include "../SerializeUtility.h"

namespace Zero
{
	void FAssetManager::Push(const std::string& ID, UAsset* Asset)
	{
		m_AllAssets.insert({ID, Asset});
	}
	void FAssetManager::Serialize(UAsset* Asset)
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << "AssetName" << YAML::Value << Asset->GetAssetName();
		Out << YAML::Key << "ObjectName" << YAML::Value << Asset->GetObjectName();
		Out << YAML::Key << "AssetFormat" << YAML::Value << Asset->GetAssetExtention();
		FSerializeUtility::SerializeVariableProperties(Out, Asset);
		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(ZConfig::GetAssestsFullPath(Asset->GetAssetName()).string(), Out.c_str());
	}


	void FAssetManager::Serialize(std::string AssetName)
	{
		auto* Asset = Fetch(AssetName);
		if (Asset != nullptr)
		{
			Serialize(Asset);
		}
	}

	UAsset* FAssetManager::Deserialize(std::filesystem::path Path)
	{
		YAML::Node Data;
		try
		{
			Data = YAML::LoadFile(Path.string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}
		std::string AssetName = Data["ObjectName"].as<std::string>();
		CORE_LOG_TRACE("Deserializing Assest '{0}'", AssetName);
		auto Iter = FObjectGlobal::GetClassInfoMap().find(AssetName);
		if (Iter != FObjectGlobal::GetClassInfoMap().end())
		{
			UAsset* Asset = nullptr;
			Asset = static_cast<UAsset*>(Iter->second.Class->CreateDefaultObject());
			FSerializeUtility::DeserializeVariablePrperties(Data, Asset);
			Asset->PostInit();
			return Asset;
		}
		return nullptr;
	}
}

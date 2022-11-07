#include "Asset.h"
#include "Utils/StringUtils.h"
#include "World/Base/ObjectGlobal.h"


namespace Zero
{
	void UAsset::Serialize()
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << "AssetName" << YAML::Value << GetObjectName();
		Out << YAML::Key << "AssetFormat" << YAML::Value << GetAssetExtention();
		FSerializeUtility::SerializeVariableProperties(Out, this);
		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(m_Path.string(), Out.c_str());
	}

	bool UAsset::Deserialize()
	{
		YAML::Node Data;
		try
		{
			Data = YAML::LoadFile(m_Path.string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}
		std::string AssetName = Data["AssetName"].as<std::string>();
		CORE_LOG_TRACE("Deserializing Assest '{0}'", AssetName);
		auto Iter = GetClassInfoMap().find(AssetName);
		if (Iter != GetClassInfoMap().end())
		{
			UAsset*  Asset = nullptr;
			Asset = static_cast<UAsset*>(Iter->second.Class->CreateDefaultObject());
			FSerializeUtility::DeserializeVariablePrperties(Data, Asset);
			Asset->PostInit();
		}
		return true;
	}
}

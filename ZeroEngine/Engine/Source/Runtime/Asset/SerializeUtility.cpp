#include "SerializeUtility.h"


namespace Zero
{
	void FSerializeUtility::ImportValue(YAML::Emitter& Out, UProperty* Property)
	{
		Out << YAML::Key << "Type" << YAML::Value << Property->GetPropertyType();
		const std::string& Type = Property->GetPropertyType();
		std::cout << "type:" << Type << std::endl;
		if (Type == "bool")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<bool>();
		else if (Type == "int")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<int>();
		else if (Type == "string" || 
			Type == "std::string" || 
			Type == "FTextureHandle" || 
			Type == "FMaterialHandle" ||
			Type == "std::filesystem::path"||
			Type == "FAssignedFile")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<std::string>();
		else if (Type == "uint32_t")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<uint32_t>();
		else if (Type == "int32_t")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<int32_t>();
		else if (Type == "ZMath::vec3")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec3>();
		else if (Type == "ZMath::vec4")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec4>();
		else if (Type == "ZMath::FColor4")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FColor4>();
		else if (Type == "ZMath::FColor3")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FColor3>();
		else if (Type == "ZMath::FRotation")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FRotation>();
		else if (Type == "float")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<float>();
		else if (Type == "double")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<double>();
		else if (Type == "FAssetHandle")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<FAssetHandle>();
	}

	void FSerializeUtility::ExporttValue(YAML::Node& Data, UProperty* Property)
	{
		std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
		YAML::Node PropertyNode = Data[PropertyNameWithClassName];
		if (PropertyNode.Type() == YAML::NodeType::Undefined)
		{
			return;
		}

		const std::string& Type = Property->GetPropertyType();
		void* PropertyData = Property->GetData();
		if (PropertyNode["Type"].as<std::string>() != Type)
		{
			return;
		}
		if (Type == "bool")
			*((bool*)PropertyData) = PropertyNode["Value"].as<bool>();
		else if (Type == "int")
			*((int*)PropertyData) = PropertyNode["Value"].as<int>();
		else if (Type == "string" ||
			Type == "std::string" ||
			Type == "FTextureHandle" ||
			Type == "FMaterialHandle" ||
			Type == "std::filesystem::path" ||
			Type == "FAssignedFile")
		{
			*((std::string*)PropertyData) = PropertyNode["Value"].as<std::string>();
		}
		else if (Type == "uint32_t")
			*((uint32_t*)PropertyData) = PropertyNode["Value"].as<uint32_t>();
		else if (Type == "int32_t")
			*((int32_t*)PropertyData) = PropertyNode["Value"].as<int32_t>();
		else if (Type == "ZMath::vec3")
			*((ZMath::vec3*)PropertyData) = PropertyNode["Value"].as<ZMath::vec3>();
		else if (Type == "ZMath::vec4")
			*((ZMath::vec4*)PropertyData) = PropertyNode["Value"].as<ZMath::vec4>();
		else if (Type == "ZMath::FColor4")
			*((ZMath::FColor4*)PropertyData) = PropertyNode["Value"].as<ZMath::FColor4>();
		else if (Type == "ZMath::FRotation")
			*((ZMath::FRotation*)PropertyData) = PropertyNode["Value"].as<ZMath::FRotation>();
		else if (Type == "float")
			*((float*)PropertyData) = PropertyNode["Value"].as<float>();
		else if (Type == "double")
			*((double*)PropertyData) = PropertyNode["Value"].as<double>();
		else if (Type == "FAssetHandle")
			*((FAssetHandle*)PropertyData) = FAssetHandle(PropertyNode["Value"].as<std::string>());
	}

	void FSerializeUtility::SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* CoreObject)
	{
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;

		Out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		while (Property != nullptr)
		{
			if (Utils::CodeReflectionTypes.contains(Property->GetPropertyType()))
			{
				std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
				Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
				ImportValue(Out, Property);
				Out << YAML::EndMap;
			}
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
		Out << YAML::EndMap;
	}

	void FSerializeUtility::DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject)
	{
		auto PropertiesNode = Data["Properties"];
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
		while (Property != nullptr)
		{
			ExporttValue(PropertiesNode, Property);
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
	}

	void FSerializeUtility::SerializeComponent(YAML::Emitter& Out, UComponent* Componnet)
	{
		Out << YAML::BeginMap;
		Out << YAML::Key << "ComopnentName" << YAML::Value << Componnet->GetObjectName();
		Out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		UProperty* Property = Componnet->GetClassCollection().HeadProperty;
		while (Property != nullptr)
		{
			if (Utils::CodeReflectionTypes.contains(Property->GetPropertyType()))
			{
				std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
				Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
				FSerializeUtility::ImportValue(Out, Property);
				Out << YAML::EndMap;
			}
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
		Out << YAML::EndMap;
		Out << YAML::EndMap;
	}


	bool FSerializeUtility::DeserializeComponents(YAML::Node& Data, UActor* Actor)
	{
		auto ComponnetNode = Data["Component"];
		auto NodeIter = ComponnetNode.begin();
		for (UComponent* Component : Actor->GetAllComponents())
		{
			if (Component->GetObjectName() != (*NodeIter)["ComopnentName"].as<std::string>())
			{
				return false;
			}
			DeserializeVariablePrperties(*NodeIter, Component);
			NodeIter++;
		}
		return true;
	}
}

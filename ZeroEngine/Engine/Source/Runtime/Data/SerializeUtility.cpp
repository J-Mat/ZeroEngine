#include "SerializeUtility.h"
#include "World/Base/ObjectGlobal.h"


namespace Zero
{
	void FSerializeUtility::ExportValue(YAML::Emitter& Out, UProperty* Property)
	{
		Out << YAML::Key << "Type" << YAML::Value << Property->GetPropertyType();
		const std::string& Type = Property->GetPropertyType();
		std::cout << "type:" << Type << std::endl;
		if (Type == "bool")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<bool>();
		else if (Type == "int" || Type == "int32_t" || FObjectGlobal::GetRegiterEnumMap().contains(Type))
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<int>();
		else if (Type == "string" ||
			Type == "std::string" ||
			Type == "FTextureHandle" ||
			Type == "FMaterialHandle" ||
			Type == "std::filesystem::path" ||
			Type == "FShaderFileHandle")
		{
			std::string Str = *Property->GetData<std::string>();
			Out << YAML::Key << "Value" << YAML::Value << Str;
		}
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
		else if (Type == "FFloatSlider")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<FFloatSlider>();
		else if (Type == "std::vector")
			ExportVectorValue(Out, Property);
		else if (Type == "std::map")
			ExportMapValue(Out, Property);
	}

	void FSerializeUtility::ExportVectorValue(YAML::Emitter& Out, UProperty* Property)
	{
		UArrayProperty* ArrayProperty = dynamic_cast<UArrayProperty*>(Property);
		Out << YAML::Key << "InValueType" << YAML::Value << ArrayProperty->GetValueType();
		Out << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

		UProperty* ValueProperty = ArrayProperty->GetClassCollection().HeadProperty;
		while (ValueProperty)
		{
			Out << YAML::BeginMap;
			ExportValue(Out, ValueProperty);
			Out << YAML::EndMap;
			ValueProperty = dynamic_cast<UProperty*>(ValueProperty->Next);
		}
			
		Out << YAML::EndSeq;
	}

	void FSerializeUtility::ExportMapValue(YAML::Emitter& Out, UProperty* Property)
	{
		UMapProperty* MapProperty = dynamic_cast<UMapProperty*>(Property);
		Out << YAML::Key << "KeyType" << YAML::Value << MapProperty->GetKeyType();
		Out << YAML::Key << "ValueType" << YAML::Value << MapProperty->GetValueType();
		Out << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

		UProperty* KeyProperty = MapProperty->GetClassCollection().HeadProperty;
		while (KeyProperty)
		{
			Out << YAML::BeginMap;

			Out << YAML::Key << "Map-Key" << YAML::Value << YAML::BeginMap;
			ExportValue(Out, KeyProperty);
			Out << YAML::EndMap;

			Out << YAML::Key << "Map-Value" << YAML::Value << YAML::BeginMap;
			UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProperty->Next);
			ExportValue(Out, ValueProperty);
			Out << YAML::EndMap;

			Out << YAML::EndMap;
			KeyProperty = dynamic_cast<UProperty*>(ValueProperty->Next);
		}

		Out << YAML::EndSeq;
	}

	void FSerializeUtility::ImportValue(YAML::Node& Data, UProperty* Property)
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
		ParseValue(PropertyNode, PropertyData, Type, Property);
	}

	void FSerializeUtility::ParseValue(YAML::Node& PropertyNode, void* PropertyData, const std::string Type, UProperty* Property)
	{
		if (Type == "bool")
			*((bool*)PropertyData) = PropertyNode["Value"].as<bool>();
		else if (Type == "int" || Type == "int32_t" || FObjectGlobal::GetRegiterEnumMap().contains(Type))
			*((int*)PropertyData) = PropertyNode["Value"].as<int>();
		else if (Type == "string" ||
			Type == "std::string" ||
			Type == "FTextureHandle" ||
			Type == "FMaterialHandle" ||
			Type == "std::filesystem::path" ||
			Type == "FShaderFileHandle")
		{
			std::string Str = PropertyNode["Value"].as<std::string>();
			*((std::string*)PropertyData) = Str;
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
		else if (Type == "ZMath::FColor3")
			*((ZMath::FColor3*)PropertyData) = PropertyNode["Value"].as<ZMath::FColor3>();
		else if (Type == "ZMath::FRotation")
			*((ZMath::FRotation*)PropertyData) = PropertyNode["Value"].as<ZMath::FRotation>();
		else if (Type == "FFloatSlider")
			*((FFloatSlider*)PropertyData) = PropertyNode["Value"].as<FFloatSlider>();
		else if (Type == "float")
			*((float*)PropertyData) = PropertyNode["Value"].as<float>();
		else if (Type == "double")
			*((double*)PropertyData) = PropertyNode["Value"].as<double>();
		else if (Type == "FAssetHandle")
			*((FAssetHandle*)PropertyData) = FAssetHandle(PropertyNode["Value"].as<std::string>());
		else if (Type == "std::vector")
			ImportVectorValue(PropertyNode, Property);
		else if (Type == "std::map")
			ImportMapValue(PropertyNode, Property);
	}


	void FSerializeUtility::ImportVectorValue(YAML::Node& Data, UProperty* Property)
	{
		UArrayProperty* ArrayProperty = static_cast<UArrayProperty*>(Property);
		std::string InValueType =  Data["InValueType"].as<std::string>(); 
		ArrayProperty->SetValueType(InValueType);
		auto Iter = Utils::CodeReflectionTypes.find(InValueType);
		CORE_ASSERT(Iter != Utils::CodeReflectionTypes.end(), "Can not find Code Reflection Type");
		uint32_t ValueSize = uint32_t(Iter->second);
		ArrayProperty->SetValueSize(ValueSize);
		auto ValueNode = Data["Value"];
		for (auto EachValue : ValueNode)
		{
			UProperty* ValueProperty = ArrayProperty->AddItem();
			std::string Type = EachValue["Type"].as<std::string>();
			void* PropertyData = ValueProperty->GetData();
			ParseValue(EachValue, PropertyData, Type, ValueProperty);
		}
		Property->GetOuter()->UpdateEditorContainerPropertyDetails(Property);
	}

	void FSerializeUtility::ImportMapValue(YAML::Node& Data, UProperty* Property)
	{
		UMapProperty* MapProperty = static_cast<UMapProperty*>(Property);

		std::string KeyType =  Data["KeyType"].as<std::string>(); 
		auto Iter = Utils::CodeReflectionTypes.find(KeyType);
		CORE_ASSERT(Iter != Utils::CodeReflectionTypes.end(), "Can not find Code Reflection Type");
		uint32_t KeySize = uint32_t(Iter->second);
		MapProperty->SetKeySize(KeySize);

		std::string ValueType =  Data["ValueType"].as<std::string>(); 
		Iter = Utils::CodeReflectionTypes.find(ValueType);
		CORE_ASSERT(Iter != Utils::CodeReflectionTypes.end(), "Can not find Code Reflection Type");
		uint32_t ValueSize = uint32_t(Iter->second);
		MapProperty->SetValueSize(ValueSize);

		auto ValueNode = Data["Value"];
		for (auto EachValue : ValueNode)
		{
			UProperty* KeyProperty = MapProperty->AddItem();
			std::string KeyType = EachValue["Map-Key"]["Type"].as<std::string>();
			void* KeyPropertyData = KeyProperty->GetData();
			ParseValue(EachValue["Map-Key"], KeyPropertyData, KeyType, KeyProperty);

			UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProperty->Next);
			std::string ValueType = EachValue["Map-Value"]["Type"].as<std::string>();
			void* ValuePropertyData = ValueProperty->GetData();
			ParseValue(EachValue["Map-Value"], ValuePropertyData, ValueType, ValueProperty);

		}
		Property->GetOuter()->UpdateEditorContainerPropertyDetails(Property);
	}

	void FSerializeUtility::SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* CoreObject)
	{
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;

		Out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		while (Property != nullptr)
		{
			std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
			Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
			ExportValue(Out, Property);
			Out << YAML::EndMap;
			Property = dynamic_cast<UProperty*>(Property->Next);
		}
		Out << YAML::EndMap;
	}

	void FSerializeUtility::DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject)
	{
		auto PropertiesNode = Data["Properties"];
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
		while (Property != nullptr)
		{
			ImportValue(PropertiesNode, Property);
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
			std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
			Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
			FSerializeUtility::ExportValue(Out, Property);
			Out << YAML::EndMap;
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
			std::cout << Component->GetObjectName() << std::endl;
			DeserializeVariablePrperties(*NodeIter, Component);
			NodeIter++;
			Component->PostInit();
		}
		return true;
	}
}

#pragma once

#include "yaml-cpp/yaml.h"
#include "World/Base/PropertyObject.h"
#include "World/Base/ArrayPropretyObject.h"
#include "World/Base/MapPropretyObject.h"
#include "World/Actor/Actor.h"
#include "World/Component/Component.h"


namespace YAML
{
	template<>
	struct convert<Zero::ZMath::vec3>
	{
		static Node encode(const Zero::ZMath::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Zero::ZMath::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Zero::ZMath::vec4>
	{
		static Node encode(const Zero::ZMath::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Zero::ZMath::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

 
	template<>
	struct convert<Zero::FFloatSlider>
	{
		static Node encode(const Zero::FFloatSlider& rhs)
		{
			Node node;
			node.push_back(rhs.Value);
			node.push_back(rhs.Min);
			node.push_back(rhs.Max);
			node.push_back(rhs.Step);
			node.push_back(rhs.bEnableEdit);
			return node;
		}

		static bool decode(const Node& node, Zero::FFloatSlider& rhs)
		{
			if (!node.IsSequence() || node.size() != 5)
				return false;

			rhs.Value = node[0].as<float>();
			rhs.Min = node[1].as<float>();
			rhs.Max = node[2].as<float>();
			rhs.Step = node[3].as<float>();
			rhs.bEnableEdit = node[4].as<bool>();
			return true;
		}
	};
}

namespace Zero
{
	static YAML::Emitter& operator<<(YAML::Emitter& Out, const Zero::ZMath::vec3& v)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return Out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& Out, const Zero::ZMath::vec4& v)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return Out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& Out, const FFloatSlider& Value)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << Value.Value << Value.Min << Value.Max << Value.Step << Value.bEnableEdit << YAML::EndSeq;
		return Out;
	}
}

namespace Zero
{
	class FSerializeUtility
	{
	public:
		static void ExportValue(YAML::Emitter& Out, UProperty* Property);
		static void ExportVectorValue(YAML::Emitter& Out, UProperty* Property);
		static void ExportMapValue(YAML::Emitter& Out, UProperty* Property);
		static void ImportValue(YAML::Node& Data, UProperty* Property);
		static void ParseValue(YAML::Node& PropertyNode, void* PropertyData, const std::string Type, UProperty* Property);
		static void ImportVectorValue(YAML::Node& Data, UProperty* Property);
		static void ImportMapValue(YAML::Node& Data, UProperty* Property);
		static void SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* CoreObject);
		static void SerializeComponent(YAML::Emitter& Out, UComponent* Componnet);
		static void DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject);
		static bool DeserializeComponents(YAML::Node& Data, UActor* Actor);
	};
}
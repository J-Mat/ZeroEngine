#pragma once

#include "yaml-cpp/yaml.h"
#include "Utils/Math/ZMath.h"
#include "World/Base/PropertyObject.h"
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
}

namespace Zero
{
	static YAML::Emitter& operator<<(YAML::Emitter& out, const Zero::ZMath::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const Zero::ZMath::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
}

namespace Zero
{
	class FSerializeUtility
	{
	public:
		static void ImportValue(YAML::Emitter& Out, UProperty* Property);
		static void ExporttValue(YAML::Node& Data, UProperty* CurrentProperty);
		static void SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* CoreObject);
		static void SerializeComponent(YAML::Emitter& Out, UComponent* Componnet);
		static void DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject);
		static bool DeserializeComponents(YAML::Node& Data, UActor* Actor);
	};
}
#pragma once
#include "Core.h"


namespace Zero
{
	enum class EShaderDataType
	{
		None,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		RGB,
		RGBA,
	};

	static uint32_t ShaderDataTypeSize(EShaderDataType Type)
	{
		switch (Type)
		{
		case Zero::EShaderDataType::Float:  return 4;
		case Zero::EShaderDataType::Float2: return 4 * 2;
		case Zero::EShaderDataType::Float3: return 4 * 3;
		case Zero::EShaderDataType::Float4: return 4 * 4;
		case Zero::EShaderDataType::Mat3:   return 4 * 3 * 3;
		case Zero::EShaderDataType::Mat4:   return 4 * 4 * 4;
		case Zero::EShaderDataType::Int:    return 4;
		case Zero::EShaderDataType::Int2:   return 4 * 2;
		case Zero::EShaderDataType::Int3:   return 4 * 3;
		case Zero::EShaderDataType::Int4:   return 4 * 4;
		case Zero::EShaderDataType::Bool:   return 1;
		case Zero::EShaderDataType::RGB:    return 4 * 3;
		case Zero::EShaderDataType::RGBA:   return 4 * 4;
		default:
			CORE_LOG_ERROR("Unknown Format!");
			return 0;
		}
	}

	uint32_t GetShaderTypeComponentCount(EShaderDataType Type) 
	{
		switch (Type)
		{
		case Zero::EShaderDataType::Float:  return 1;
		case Zero::EShaderDataType::Float2: return 2;
		case Zero::EShaderDataType::Float3: return 3;
		case Zero::EShaderDataType::Float4: return 4;
		case Zero::EShaderDataType::Mat3:   return 3 * 3;
		case Zero::EShaderDataType::Mat4:   return 4 * 4;
		case Zero::EShaderDataType::Int:    return 1;
		case Zero::EShaderDataType::Int2:   return 2;
		case Zero::EShaderDataType::Int3:   return 3;
		case Zero::EShaderDataType::Int4:   return 4;
		case Zero::EShaderDataType::Bool:   return 1;
		case Zero::EShaderDataType::RGB:    return 3;
		case Zero::EShaderDataType::RGBA:   return 4;
		default:
			CORE_LOG_ERROR("Unknown Format!");
			return 0;
		}
	}
	
	struct FBufferElement
	{
		std::string m_Name;
		EShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool bNormaliezd;
		FBufferElement(std::string _Name, EShaderDataType _Type, bool _bNormaliezd = false)
			: m_Name(_Name)
			, Type(_Type)
			, Size(ShaderDataTypeSize(Type))
			, Offset(0)
			, bNormaliezd(_bNormaliezd)
		{}

		bool operator==(const FBufferElement& Other) const
		{
			return (m_Name == Other.m_Name) && (Type == Other.Type);
		}
		bool operator!=(const FBufferElement& Other) const
		{
			return !(*this == Other);
		}
	};
}
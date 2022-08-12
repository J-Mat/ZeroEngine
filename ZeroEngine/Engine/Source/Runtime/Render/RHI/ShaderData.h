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
		case Zero::EShaderDataType::None:
			return 0;
		case Zero::EShaderDataType::Float:
			break;
		case Zero::EShaderDataType::Float2:
			break;
		case Zero::EShaderDataType::Float3:
			break;
		case Zero::EShaderDataType::Float4:
			break;
		case Zero::EShaderDataType::Mat3:
			break;
		case Zero::EShaderDataType::Mat4:
			break;
		case Zero::EShaderDataType::Int:
			break;
		case Zero::EShaderDataType::Int2:
			break;
		case Zero::EShaderDataType::Int3:
			break;
		case Zero::EShaderDataType::Int4:
			break;
		case Zero::EShaderDataType::Bool:
			break;
		case Zero::EShaderDataType::RGB:
			break;
		case Zero::EShaderDataType::RGBA:
			break;
		default:
			break;
		}
	}
}
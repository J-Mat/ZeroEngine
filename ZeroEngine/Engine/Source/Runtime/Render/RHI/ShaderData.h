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
	
	enum class EShaderResourceType
	{
		Texture2D,
		Cubemap,
		RenderTarget,
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

	static uint32_t GetShaderTypeComponentCount(EShaderDataType Type)
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


	class FConstantBufferLayout
	{
	public:
		FConstantBufferLayout() {}
		FConstantBufferLayout(const std::initializer_list<FBufferElement>& Elements)
			: m_Elements(Elements)
		{
			CalculateOffsetsAndStride();
		}
		inline const std::vector<FBufferElement>& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }
		std::vector<FBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FBufferElement>::iterator end() { return m_Elements.end(); }

		static FConstantBufferLayout PerObjectConstants;
		static FConstantBufferLayout PerCameraConstants;
		static FConstantBufferLayout PerFrameConstants;
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t Offset = 0;
			m_Stride = 0;
			for (auto& Element : m_Elements)
			{
				Element.Offset = Offset;
				Offset += Element.Size;
				m_Stride += Element.Size;
			}
		}
		std::vector<FBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	struct FTextureTableElement
	{
		EShaderResourceType Type;
		std::string Name;
	};
	
	class FShaderResourceLayout
	{
	public:
		FShaderResourceLayout() {}
		FShaderResourceLayout(const std::initializer_list<FTextureTableElement>& elements)
			:m_Elements(elements)
		{
		}

		size_t GetSrvCount()
		{
			return m_Elements.size();
		}

		inline const std::vector<FTextureTableElement>& GetElements() const { return m_Elements; }

		std::vector<FTextureTableElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FTextureTableElement>::iterator end() { return m_Elements.end(); }

	private:
		std::vector<FTextureTableElement> m_Elements;
	};


	struct FComputeOutputElement
	{
		EShaderResourceType Type;
		std::string Name;
	};

	class FComputeOutputLayout
	{
	public:
		FComputeOutputLayout() {}
		FComputeOutputLayout(const std::initializer_list<FComputeOutputElement>& elements)
			:m_Elements(elements)
		{
		}

		size_t SrvCount()
		{
			return m_Elements.size();
		}

		inline const std::vector<FComputeOutputElement>& GetElements() const { return m_Elements; }

		std::vector<FComputeOutputElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FComputeOutputElement>::iterator end() { return m_Elements.end(); }

	private:
		std::vector<FComputeOutputElement> m_Elements;
	};
}
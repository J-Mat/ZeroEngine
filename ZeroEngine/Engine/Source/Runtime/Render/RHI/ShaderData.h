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

	enum class EShaderType
	{
		ST_VERTEX,
		ST_PIXEL,
		ST_COMPUTE
	};

	static uint32_t ShaderDataTypeSize(EShaderDataType Type)
	{
		switch (Type)
		{
		case Zero::EShaderDataType::Float:  return 4;
		case Zero::EShaderDataType::Float2: return 4 * 2;
		case Zero::EShaderDataType::Float3: return 4 * 3;
		case Zero::EShaderDataType::Float4: return 4 * 4;
		case Zero::EShaderDataType::Mat3:   return 4 * 3 * 4;
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

	struct FShaderParameter
	{
		std::string ResourceName;
		EShaderType ShaderType;
		UINT BindPoint;
		UINT RegisterSpace;
	};
	
	struct FCBVariableItem
	{
		std::string VariableName = "";
		std::string VariableTypeName = "";
		EShaderDataType ShaderDataType;
	};

	struct FShaderCBVParameter : FShaderParameter
	{
		std::vector<FCBVariableItem> VariableList;
	};

	struct FShaderSRVParameter : FShaderParameter
	{
		UINT BindCount;
		EShaderResourceType ShaderResourceType;
	};

	struct FShaderUAVParameter : FShaderParameter
	{
		UINT BindCount;
	};
	
	struct FBufferElement
	{
		std::string Name;
		EShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool bNormaliezd;
		FBufferElement(EShaderDataType _Type, std::string _Name, bool _bNormaliezd = false)
			: Type(_Type)
			, Name(_Name)
			, Size(ShaderDataTypeSize(Type))
			, Offset(0)
			, bNormaliezd(_bNormaliezd)
		{}

		bool operator==(const FBufferElement& Other) const
		{
			return (Name == Other.Name) && (Type == Other.Type);
		}
		bool operator!=(const FBufferElement& Other) const
		{
			return !(*this == Other);
		}

		static FBufferElement s_Pos;
		static FBufferElement s_Normal;
		static FBufferElement s_Tangent;
		static FBufferElement s_Tex;
	};


	class FConstantBufferLayout
	{
		friend class FShader;
	public:
		FConstantBufferLayout() {}
		FConstantBufferLayout(const std::string BufferName, int32_t BindPoint,  const std::initializer_list<FBufferElement>& Elements)
			: m_BufferName(BufferName)
			, m_BindPoint(BindPoint)
			, m_Elements(Elements)
		{
			CalculateOffsetsAndStride();
		}
		const std::vector<FBufferElement>& GetElements() const { return m_Elements; }
		const uint32_t GetStride() const { return m_Stride; }
		const int32_t GetBindPoint() const 
		{ 
			CORE_ASSERT(m_BindPoint >= 0, "BindPoint is not valid!")
			return m_BindPoint; 
		}
		const std::string& GetBufferName() const { return m_BufferName; }
		std::vector<FBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FBufferElement>::iterator end() { return m_Elements.end(); }

		static FConstantBufferLayout s_PerObjectConstants;
		static FConstantBufferLayout s_PerCameraConstants;
		static FConstantBufferLayout s_GlobalConstants;
		static FConstantBufferLayout s_InvalidContants;

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
	private:
		std::vector<FBufferElement> m_Elements;
		std::string m_BufferName;
		int32_t m_BindPoint = 0;
		uint32_t m_Stride = 0;
	};

	struct FTextureTableElement
	{
		EShaderResourceType Type;
		std::string ResourceName;
		uint32_t TextureNum = 1;
		uint32_t BindPoint = 0;
	};
	
	class FShaderResourceLayout
	{
		friend class FShader;
	public:
		FShaderResourceLayout() {}
		FShaderResourceLayout(const std::initializer_list<FTextureTableElement>& Elements)
			:m_Elements(Elements)
		{
		}

		size_t GetSrvCount()
		{
			return m_Elements.size();
		}

		const std::vector<FTextureTableElement>& GetElements() const { return m_Elements; }

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
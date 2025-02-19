#pragma once
#include "Core.h"
#include "EnumUtil.h"


namespace Zero
{
	enum class EShaderStage
	{
		VS,
		PS,
		CS,
		HS,
		DS,
		GS,
		LIB,
		MS,
		AS,
		ShaderCount
	};
	enum EShaderModel
	{
		SM_5_1,
		SM_6_0,
		SM_6_1,
		SM_6_2,
		SM_6_3,
		SM_6_4,
		SM_6_5,
		SM_6_6
	};
	class FShaderMacro
	{	
	public:
		void SetDefine(const std::string& Name, const std::string& Definition);
		bool operator == (const FShaderMacro& Other) const;
	private:
		std::unordered_map<std::string, std::string> DefinesMap;
	};

	enum EShaderCompilerFlagBit
	{
		ShaderCompilerFlag_None = 0,
		ShaderCompilerFlag_Debug = 1 << 0,
		ShaderCompilerFlag_DisableOptimization = 1 << 1
	};
	DEFINE_ENUM_BIT_OPERATORS(EShaderCompilerFlagBit);

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
		Texture2D_Uav,
		Buffer,
		Buffer_Uav,
	};

	enum class EShaderSampler
	{
		PointWarp, 
		PointClamp, 
		LinearWarp, 
		LinearClamp, 
		AnisotropicWarp, 
		AnisotropicClamp, 
		Shadow
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

	static uint32_t GetShaderStageComponentCount(EShaderDataType Type)
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
		EShaderStage ShaderStage;
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
		EShaderResourceType ShaderResourceType;
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
		FConstantBufferLayout(const std::string BufferName, int32_t BindPoint,  const std::initializer_list<FBufferElement>& Elements, bool bDynamic)
			: m_BufferName(BufferName)
			, m_BindPoint(BindPoint)
			, m_Elements(Elements)
			, m_bDynamic(bDynamic)
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
		void SetDynamic(bool bDynamic) { m_bDynamic = bDynamic; }
		bool GetDynamic() { return m_bDynamic; };
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
		int32_t m_RegisterSpace = 0;
		uint32_t m_Stride = 0;
		bool m_bDynamic = false;
	};

	struct FSRVElement
	{
		EShaderResourceType Type;
		std::string ResourceName;
		uint32_t TextureNum = 1;
		uint32_t BindPoint = 0;
		uint32_t RegisterSpace = 0;
		bool bWillBeModified = false;
	};
	
	class FSRVResourceLayout
	{
		friend class FShader;
	public:
		FSRVResourceLayout() {}
		FSRVResourceLayout(const std::initializer_list<FSRVElement>& Elements)
			:m_Elements(Elements)
		{
		}

		size_t GetSrvCount()
		{
			return m_Elements.size();
		}

		const std::vector<FSRVElement>& GetElements() const { return m_Elements; }

		std::vector<FSRVElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FSRVElement>::iterator end() { return m_Elements.end(); }

	private:
		std::vector<FSRVElement> m_Elements;
	};


	struct FUAVElement
	{
		EShaderResourceType Type;
		std::string ResourceName;
		uint32_t TextureNum = 1;
		uint32_t BindPoint = 0;
		uint32_t RegisterSpace = 0;
		bool bWillBeModified = false;
	};

	class FUAVResoureceLayout
	{
		friend class FShader;
	public:
		FUAVResoureceLayout() {}
		FUAVResoureceLayout(const std::initializer_list<FUAVElement>& elements)
			:m_Elements(elements)
		{
		}

		size_t GetUavCount()
		{
			return m_Elements.size();
		}

		inline const std::vector<FUAVElement>& GetElements() const { return m_Elements; }

		std::vector<FUAVElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<FUAVElement>::iterator end() { return m_Elements.end(); }

	private:
		std::vector<FUAVElement> m_Elements;
	};
}
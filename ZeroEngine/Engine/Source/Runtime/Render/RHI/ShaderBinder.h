#pragma once

#include "Core.h"
#include "ShaderData.h"

namespace Zero
{
	class FShaderBinderDesc
	{
	public:
		FShaderBinderDesc() = default;
		FShaderBinderDesc(
			const std::vector<FConstantBufferLayout>& ConstantBufferLayouts,
			FShaderResourceLayout& ShaderResourceLayout = FShaderResourceLayout(),
			const std::vector<FComputeOutputLayout>& ComputeOutputLayouts = std::vector<FComputeOutputLayout>())
			: m_ConstantBufferLayouts(ConstantBufferLayouts)
			, m_TextureBufferLayout(ShaderResourceLayout)
			, m_ComputeOutputLayouts(ComputeOutputLayouts)
		{
		}
		~FShaderBinderDesc() {};
		size_t GetConstantBufferCount() const { return m_ConstantBufferLayouts.size(); }
		std::vector<FConstantBufferLayout> m_ConstantBufferLayouts;
		FShaderResourceLayout m_TextureBufferLayout;
		std::vector<FComputeOutputLayout>  m_ComputeOutputLayouts;
	
	public:
		int32_t m_PerObjIndex = Utils::InvalidIndex;
		int32_t m_CameraIndex = Utils::InvalidIndex;
		int32_t m_ConstantIndex = Utils::InvalidIndex;
		int32_t m_MaterialIndex = Utils::InvalidIndex;
	};


	struct FShaderConstantItem
	{
		std::string Name;
		EShaderDataType Type;
		uint32_t Offset;
	};

	class FConstantsMapper
	{
	public:
		void InsertConstant(const FBufferElement& Element);
		bool FetchConstant(std::string Name, FShaderConstantItem& Buffer);
		using iterator = std::unordered_map<std::string, FShaderConstantItem>::iterator;
		iterator begin() { return m_Mapper.begin(); }
		iterator end() { return m_Mapper.end(); }
	private:
		std::unordered_map<std::string, FShaderConstantItem> m_Mapper;
	};

	struct FShaderResourceItem
	{
		std::string Name;
		EShaderResourceType Type;
		uint32_t SRTIndex;
		uint32_t Offset;
		uint32_t ResNum = 1;
		std::string TextureID = "";
		uint32_t SelectedMip = 0;
	};

	class FResourcesMapper
	{
	public:
		void InsertResource(const FShaderResourceItem& Element);
		bool FetchResource(std::string name, FShaderResourceItem& Buffer);
		bool SetTextureID(std::string Name, EShaderResourceType Type = EShaderResourceType::Texture2D, unsigned int Mip = 0);

		using iterator = std::unordered_map<std::string, FShaderResourceItem>::iterator;
		iterator begin() { return m_Mapper.begin(); }
		iterator end() { return m_Mapper.end(); }

	private:
		std::unordered_map<std::string, FShaderResourceItem> m_Mapper;
	};
	
	struct FShaderConstantsDesc
	{
		uint32_t Size = -1;
		FConstantsMapper Mapper;

		FConstantsMapper::iterator begin() { return Mapper.begin(); }
		FConstantsMapper::iterator end() { return Mapper.end(); }
	};

	class IShaderBinder;
	class IShaderConstantsBuffer
	{
	public:
		IShaderConstantsBuffer(FShaderConstantsDesc& Desc) : m_Desc(Desc) {}
		virtual ~IShaderConstantsBuffer() = default;

		virtual void SetInt(const std::string& Name, const int& Value) = 0;
		virtual void SetFloat(const std::string& Name, const float& Value) = 0;
		virtual void SetFloat2(const std::string& Name, const ZMath::vec2& Value) = 0;
		virtual void SetFloat3(const std::string& Name, const ZMath::vec3& Value) = 0;
		virtual void SetFloat4(const std::string& Name, const ZMath::vec4& Value) = 0;
		virtual void SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value) = 0;

		virtual void GetInt(const std::string& Name, int& Value) = 0;
		virtual void GetFloat(const std::string& Name, float& Value) = 0;
		virtual void GetFloat3(const std::string& Name, ZMath::vec3& Value) = 0;
		virtual void GetFloat4(const std::string& Name, ZMath::vec4& Value) = 0;
		virtual void GetMatrix4x4(const std::string& Name, ZMath::mat4& Value) = 0;

		virtual float* PtrFloat(const std::string& Name) = 0;
		virtual float* PtrFloat3(const std::string& Name) = 0;
		virtual float* PtrFloat4(const std::string& Name) = 0;
		virtual float* PtrMatrix4x4(const std::string& Name) = 0;


		virtual void UploadDataIfDirty() = 0;
		virtual void SetDirty() = 0;
		virtual void Test() {};

	protected:
		FShaderConstantsDesc& m_Desc;
	};

	struct FShaderResourcesDesc
	{
		uint32_t Size = 0;
		FResourcesMapper Mapper;

		FResourcesMapper::iterator begin() { return Mapper.begin(); }
		FResourcesMapper::iterator end() { return Mapper.end(); }
	};

	class FTexture2D;
	class FTextureCubemap;
	class IShaderResourcesBuffer
	{
	public:
		IShaderResourcesBuffer(FShaderResourcesDesc& Desc) :m_Desc(Desc) {}
		virtual ~IShaderResourcesBuffer() = default;
		virtual FShaderResourcesDesc* GetShaderResourceDesc() = 0;
		virtual void SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture) = 0;
		virtual void SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture) = 0;

		virtual void UploadDataIfDirty() = 0;
	protected:
		FShaderResourcesDesc& m_Desc;
	};
	
	class IRootSignature;
	class IShaderBinder
	{
	public:
		IShaderBinder(FShaderBinderDesc& Desc);
		virtual ~IShaderBinder() { m_ShaderConstantDescs.clear(); }
		virtual void BindConstantsBuffer(uint32_t Slot, IShaderConstantsBuffer* Buffer) = 0;
		virtual Ref<FShaderConstantsDesc> GetShaderConstantsDesc(uint32_t Slot) 
		{ 
			return Slot < m_ShaderConstantDescs.size() ? m_ShaderConstantDescs[Slot] : nullptr;
		}
		virtual Ref<FShaderResourcesDesc> GetShaderResourcesDesc() { return m_ShaderResourceDesc; }
		virtual IRootSignature* GetRootSignature() { return nullptr; }
		virtual void Bind() = 0;
		const FShaderBinderDesc& GetBinderDesc() { return m_Desc; }
	protected:
		void InitMappers();
		FConstantsMapper m_ConstantsMapper;
		FResourcesMapper m_ResourcesMapper;

		std::vector<Ref<FShaderConstantsDesc>> m_ShaderConstantDescs;
		Ref<FShaderResourcesDesc> m_ShaderResourceDesc;
		FShaderBinderDesc& m_Desc;
	};
}




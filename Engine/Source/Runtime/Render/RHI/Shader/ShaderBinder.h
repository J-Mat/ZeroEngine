#pragma once

#include "Core.h"
#include "ShaderData.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FShaderBinderDesc
	{
	public:
		FShaderBinderDesc() = default;
		FShaderBinderDesc(
			std::vector<EShaderSampler>& ShaderSamplers,
			std::vector<FConstantBufferLayout>& ConstantBufferLayouts,
			FSRVResourceLayout& ShaderResourceLayout = FSRVResourceLayout(),
			FUAVResoureceLayout& ReadWriteResourceLayout = FUAVResoureceLayout())
			: m_ShaderSamplers(ShaderSamplers)
			, m_ConstantBufferLayouts(ConstantBufferLayouts)
			, m_SRVResourceLayout(ShaderResourceLayout)
			, m_UAVResourceLayout(ReadWriteResourceLayout)
		{
		}
		~FShaderBinderDesc() {};
		const FConstantBufferLayout& GetConstantBufferLayoutByName (const std::string& ResourceName) const;
		const FSRVResourceLayout& GetTextureResourceLayout() const { return m_SRVResourceLayout; }
		const FUAVResoureceLayout& GetReadWriteResourceLayout() const { return m_UAVResourceLayout; }
		size_t GetConstantBufferCount() const { return m_ConstantBufferLayouts.size(); }
		std::vector<FConstantBufferLayout> m_ConstantBufferLayouts;
		FSRVResourceLayout m_SRVResourceLayout = FSRVResourceLayout();
		FUAVResoureceLayout  m_UAVResourceLayout;
		std::vector<EShaderSampler> m_ShaderSamplers;
		void MapCBBufferIndex();
	
	public:
		int32_t m_PerObjIndex = Utils::InvalidIndex;
		int32_t m_CameraIndex = Utils::InvalidIndex;
		int32_t m_GloabalConstantIndex = Utils::InvalidIndex;
		int32_t m_MaterialIndex = Utils::InvalidIndex;

		ERenderPassType m_RenderPassType;
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
		uint32_t ParamsIndex;
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
		int32_t Size = -1;
		FConstantsMapper Mapper;
		bool bDynamic = false;

		FConstantsMapper::iterator begin() { return Mapper.begin(); }
		FConstantsMapper::iterator end() { return Mapper.end(); }
	};

	class IShaderBinder;
	class IShaderConstantsBuffer
	{
	public:
		IShaderConstantsBuffer(FShaderConstantsDesc& Desc) : m_Desc(Desc) {}
		virtual ~IShaderConstantsBuffer() = default;

		virtual void SetInt(const std::string& Name, int Value) = 0;
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
		virtual void PreDrawCall() {};

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
	class FTextureCube;
	class FBuffer;
	struct FCommandListHandle;
	class IShaderResourcesBuffer
	{
	public:
		IShaderResourcesBuffer(FShaderResourcesDesc& Desc) :m_Desc(Desc) {}
		virtual ~IShaderResourcesBuffer() = default;
		virtual FShaderResourcesDesc* GetShaderResourceDesc() = 0;
		virtual void SetTexture2D(const std::string& Name, FTexture2D* Texture) = 0;
		virtual void SetBuffer(const std::string& Name, FBuffer* Buffer) = 0;
		virtual void SetTexture2DArray(const std::string& Name, std::vector<FTexture2D*> Textures) = 0;
		virtual void SetTextureCube(const std::string& Name, FTextureCube* Texture) = 0;
		virtual void SetTextureCubemapArray(const std::string& Name, const std::vector<Ref<FTextureCube>>& TextureCubes) = 0;

		virtual void SetTexture2D_Uav(const std::string& Name, FTexture2D* Texture) = 0;
		virtual void SetBuffer_Uav(const std::string& Name, FBuffer* Buffer)= 0;

		virtual void UploadDataIfDirty(FCommandListHandle CommandListHandle, ERenderPassType RenderPassUsage) = 0;
	protected:
		FShaderResourcesDesc& m_Desc;
	};
	
	class FRootSignature;
	class IShaderBinder
	{
	public:
		IShaderBinder(FShaderBinderDesc& Desc);
		virtual ~IShaderBinder() { m_ShaderConstantDescs.clear(); }
		virtual void BindConstantsBuffer(FCommandListHandle CommandListHandle, int32_t Slot, IShaderConstantsBuffer* Buffer) = 0;
		virtual Ref<FShaderConstantsDesc> GetShaderConstantsDesc(int32_t Slot) 
		{ 
			return Slot < m_ShaderConstantDescs.size() ? m_ShaderConstantDescs[Slot] : nullptr;
		}
		virtual Ref<FShaderResourcesDesc> GetShaderResourcesDesc() { return m_ShaderResourceDesc; }
		virtual FRootSignature* GetRootSignature() { return nullptr; }
		virtual void Bind(FCommandListHandle CommandListHandle, ERenderPassType RenderPassUsage) = 0;
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




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
			const std::vector<FShaderResourceLayout>& ShaderResourceLayouts,
			const std::vector<FComputeOutputLayout>& ComputeOutputLayouts = std::vector<FComputeOutputLayout>())
			: m_ConstantBufferLayouts(ConstantBufferLayouts)
			, m_TextureBufferLayouts(ShaderResourceLayouts)
			, m_ComputeOutputLayouts(ComputeOutputLayouts)
		{
		}
		~FShaderBinderDesc() {};
		size_t GetConstantBufferCount() const { return m_ConstantBufferLayouts.size(); }
		size_t GetTextureBufferCount() const { return m_TextureBufferLayouts.size(); }
		std::vector<FConstantBufferLayout> m_ConstantBufferLayouts;
		std::vector<FShaderResourceLayout> m_TextureBufferLayouts;
		std::vector<FComputeOutputLayout>  m_ComputeOutputLayouts;
	};


	struct FShaderConstantItem
	{
		std::string Name;
		EShaderDataType Type;
		int CBIndex;
		uint32_t Offset;
	};

	class FConstantsMapper
	{
	public:
		void InsertConstant(const FBufferElement& Element, int CBIndex);
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
		int SRTIndex;
		int Offset;
		std::string TextureID;
		int SelectedMip = 0;
	};

	class FResourcesMapper
	{
	public:
		void InsertResource(const FShaderResourceItem& Element);
		bool FetchResource(std::string name, FShaderResourceItem& Buffer);
		bool SetTextureID(std::string Name, std::string ID, EShaderResourceType Type = EShaderResourceType::Texture2D, unsigned int Mip = 0);

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
	class ShaderBinder;
	class ShaderConstantsBuffer
	{
	public:
		virtual ~ShaderConstantsBuffer() = default;

		virtual void SetInt(const std::string& name, const int& value) = 0;
		virtual void SetFloat(const std::string& name, const float& value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMatrix4x4(const std::string& name, const glm::mat4& value) = 0;

		virtual void GetInt(const std::string& name, int& value) = 0;
		virtual void GetFloat(const std::string& name, float& value) = 0;
		virtual void GetFloat3(const std::string& name, glm::vec3& value) = 0;
		virtual void GetFloat4(const std::string& name, glm::vec4& value) = 0;
		virtual void GetMatrix4x4(const std::string& name, glm::mat4& value) = 0;

		virtual float* PtrFloat(const std::string& name) = 0;
		virtual float* PtrFloat3(const std::string& name) = 0;
		virtual float* PtrFloat4(const std::string& name) = 0;
		virtual float* PtrMatrix4x4(const std::string& name) = 0;


		virtual void UploadDataIfDirty(ShaderBinder* m_ShaderBinder) = 0;
		virtual void SetDirty() = 0;

	protected:
		virtual void InitConstant() = 0;
	};

	struct FShaderResourcesDesc
	{
		uint32_t Size = -1;
		FResourcesMapper Mapper;

		FResourcesMapper::iterator begin() { return Mapper.begin(); }
		FResourcesMapper::iterator end() { return Mapper.end(); }
	};

	class FTexture2D;
	class FRenderTarget;
	class FTextureCubemap;
	class FShaderResourcesBuffer
	{
	public:
		virtual ~FShaderResourcesBuffer() = default;
		virtual FShaderResourcesDesc* GetShaderResourceDesc() = 0;
		virtual void SetTexture2D(const std::string& name, Ref<FTexture2D> texture) = 0;
		virtual void SetTexture2D(const std::string& name, FRenderTarget* texture) = 0;
		virtual void SetTextureCubemap(const std::string& name, Ref<FTextureCubemap> texture) = 0;

		virtual void UploadDataIfDirty(ShaderBinder* shaderBinder) = 0;
	};
	
	class FShaderBinder
	{
	public:
		virtual ~FShaderBinder() { delete[] m_ShaderConstantDescs; }
		virtual void BindConstantsBuffer(unsigned int Slot, ShaderConstantsBuffer& buffer) = 0;
		virtual FShaderConstantsDesc* GetShaderConstantsDesc(unsigned int slot) { return &m_ShaderConstantDescs[slot]; }
		virtual FShaderResourcesDesc* GetShaderResourcesDesc() { return &m_ShaderResourceDescs; }
	protected:
		void InitMappers(const FShaderBinderDesc& Desc);
		FConstantsMapper m_ConstantsMapper;
		FResourcesMapper m_ResourcesMapper;

		FShaderConstantsDesc* m_ShaderConstantDescs;
		FShaderResourcesDesc* m_ShaderResourceDescs;
	};
}




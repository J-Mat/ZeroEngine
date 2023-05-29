#pragma once
#include "Core.h"
#include "Render/RenderConfig.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "../Texture.h"
#include "../Buffer.h"

namespace Zero
{
	struct FPipelineStateDesc;

	class FShaderSamplerParameter : public FShaderParameter
	{};

	struct FShaderDesc
	{
		std::string FileName;
		uint32_t ShaderID;
		bool bUseAlphaBlending = false;
		FVertexBufferLayout VertexBufferLayout = FVertexBufferLayout::s_DefaultVertexLayout;
		int32_t NumRenderTarget = 1;
		FFrameBufferTexturesFormats Formats = {
			EResourceFormat::R8G8B8A8_UNORM,
		};
		
		bool bNeedDetph = true;
		EResourceFormat DepthFormat = EResourceFormat::D24_UNORM_S8_UINT;

		EShaderModel Model = SM_6_0;
		FShaderMacro ShaderMacro;
		bool bCreateVS = true;
		std::string EntryPoint[int32_t(EShaderStage::ShaderCount)] = { "VS", "PS", "CS"};
		bool bCreatePS = true;
		EShaderCompilerFlagBit Flags = ShaderCompilerFlag_Debug;
		std::unordered_map<std::string, std::string> DefinesMap;

		uint32_t BlockSize_X = 8;
		uint32_t BlockSize_Y = 8;
		uint32_t BlockSize_Z = 8;
		
		void SetDefine(const std::string& Name, const std::string& Value)
		{
			DefinesMap.insert_or_assign(Name , Value);
		}
	};

		
	using FShaderBlob = std::vector<uint8_t>;

	class FShader
	{
	public:
		FShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		FShader(const FShaderDesc& Desc);
		virtual ~FShader() {}
		virtual void Use(FCommandListHandle CommandList) = 0;
		virtual void CreateBinder() = 0;

		virtual Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }

		const std::map<std::pair<uint32_t, uint32_t>, FShaderCBVParameter>& GetCBVParams() const { return m_CBVParams; }
		const std::map<std::pair<uint32_t, uint32_t>, FShaderSRVParameter>& GetSRVParams() const { return m_SRVParams; }
		
		void GenerateShaderDesc();

		void SetBytecode(EShaderStage ShaderStage, void const* Data, size_t Size)
		{
			m_ByteCode[uint32_t(ShaderStage)].resize(Size);
			memcpy(m_ByteCode[uint32_t(ShaderStage)].data(), Data, Size);
		}

		void* GetPointer(EShaderStage ShaderStage) const
		{
			return !m_ByteCode[uint32_t(ShaderStage)].empty() ? (void*)m_ByteCode[uint32_t(ShaderStage)].data() : nullptr;
		}

		size_t GetLength(EShaderStage ShaderStage) const
		{
			return m_ByteCode[uint32_t(ShaderStage)].size();
		}

		std::set<std::string>& GetAllIncludeFiles() { return m_IncludeFiles; }

		const FShaderDesc& GetDesc() { return m_ShaderDesc; }

		virtual void Compile() = 0;

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;

		std::map<std::pair<uint32_t, uint32_t>, FShaderCBVParameter> m_CBVParams;
		std::map<std::pair<uint32_t, uint32_t>, FShaderSRVParameter> m_SRVParams;
		std::map<std::pair<uint32_t, uint32_t>, FShaderUAVParameter> m_UAVParams;
		std::vector<FShaderSamplerParameter> m_SamplerParams;

		std::set<std::string> m_IncludeFiles;
		FShaderBlob m_ByteCode[uint32_t(EShaderStage::ShaderCount)] = {};
	};
}
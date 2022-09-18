#pragma once
#include "Core.h"
#include "Render/RHI/ShaderBinder.h"
#include "Platform/DX12/MemoryManage/FrameResource.h"
#include "Render/RHI/RootSignature.h"
#include "Platform/DX12/DX12RootSignature.h"

namespace Zero
{
	class FDX12Device;
	class FDynamicDescriptorHeap;
	class FDX12ShaderConstantsBuffer : public IShaderConstantsBuffer
	{
	public:
		FDX12ShaderConstantsBuffer(FShaderConstantsDesc& Desc);

		virtual void SetInt(const std::string& Name, const int& Value);
		virtual void SetFloat(const std::string& Name, const float& Vaue);
		virtual void SetFloat2(const std::string& Name, const ZMath::vec2& Value);
		virtual void SetFloat3(const std::string& Name, const ZMath::vec3& Value);
		virtual void SetFloat4(const std::string& Name, const ZMath::vec4& Value);
		virtual void SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value);

		virtual void GetInt(const std::string& Name, int& Value);
		virtual void GetFloat(const std::string& Name, float& Value);
		virtual void GetFloat3(const std::string& Name, ZMath::vec3& Value);
		virtual void GetFloat4(const std::string& Name, ZMath::vec4& Value);
		virtual void GetMatrix4x4(const std::string& Name, ZMath::mat4& Value);

		virtual float* PtrFloat(const std::string& Name);
		virtual float* PtrFloat3(const std::string& Name);
		virtual float* PtrFloat4(const std::string& Name);
		virtual float* PtrMatrix4x4(const std::string& Name);

		virtual void UploadDataIfDirty();
		virtual void SetDirty() { m_bIsDirty = true; }
		
		Ref<FFrameResourceBuffer> GetFrameResourceBuffer() { return m_ConstantsTableBuffer; }

	private:
		FConstantsMapper& m_ConstantsMapper;
		Ref<FFrameResourceBuffer> m_ConstantsTableBuffer = nullptr;
		bool m_bIsDirty = true;
	};

	class FDX12ShaderResourcesBuffer : public IShaderResourcesBuffer
	{
	public:
		FDX12ShaderResourcesBuffer(FDX12Device& Device, FShaderResourcesDesc& Desc, FDX12RootSignature* RootSignature);
		virtual FShaderResourcesDesc* GetShaderResourceDesc();
		virtual void SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture) override;
		virtual void SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture) override;

		virtual void UploadDataIfDirty() override;
	private:
		FDX12Device& m_Device;
		Ref<FDynamicDescriptorHeap> m_SrvDynamicDescriptorHeap;
		Ref<FDynamicDescriptorHeap> m_SamplerDynamicDescriptorHeap;
		bool m_bIsDirty = true;
	};

	class FDX12ShaderBinder : public IShaderBinder
	{
	public:
		FDX12ShaderBinder(FDX12Device &Device, FShaderBinderDesc& Desc);
		virtual ~FDX12ShaderBinder();
		virtual IRootSignature* GetRootSignature() { return m_RootSignature.get(); }
		virtual void BindConstantsBuffer(uint32_t Slot, IShaderConstantsBuffer* Buffer) override;
		virtual void Bind() override;
	private:
		void BuildRootSignature();
		void BuildDynamicHeap();
	private:
		FDX12Device& m_Device;
		Ref<FDX12RootSignature> m_RootSignature;
	};
}

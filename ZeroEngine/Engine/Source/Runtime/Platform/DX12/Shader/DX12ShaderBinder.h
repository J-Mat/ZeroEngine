#pragma once
#include "Core.h"
#include "Render/RHI/ShaderBinder.h"
#include "Platform/DX12/MemoryManage/FrameResource.h"

namespace Zero
{
	class FDX12Device;
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

	private:
		FConstantsMapper& m_ConstantsMapper;
		Ref<FFrameResourceBuffer> m_ConstantsTableBuffer = nullptr;
		bool m_bIsDirty = true;
	};

	class FDX12ShaderBinder : public IShaderBinder
	{
	public:
		FDX12ShaderBinder(FDX12Device &Device, FShaderBinderDesc& Desc);
		virtual ~FDX12ShaderBinder();
	private:
		void BuildRootSignature();
	private:
		FDX12Device& m_Device;
	};
}

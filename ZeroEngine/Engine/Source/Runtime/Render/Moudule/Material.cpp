#include "Material.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/RHI/Texture.h"
#include "World/World.h"
#include "World/Actor/CameraActor.h"

namespace Zero
{

	FMaterial::FMaterial(IDevice* Device)
		: m_Device(Device)
	{
	}

	FMaterial::~FMaterial()
	{
	}
	void FMaterial::SetPass()
	{
		m_Shader->GetBinder()->BindConstantsBuffer(ERootParameters::MaterialCB, m_ConstantsBuffer.get());
		UCameraActor* Camera = UWorld::GetCurrentWorld()->GetCameraActor();
		m_Shader->GetBinder()->BindConstantsBuffer(ERootParameters::CameraCB, Camera->GetConstantBuffer().get());
		m_Shader->Use();
	}

	void FMaterial::OnDrawCall()
	{
		m_ConstantsBuffer->UploadDataIfDity();
		m_ResourcesBuffer->UploadDataIfDirty();
	}
	void FMaterial::SetShader(Ref<IShader> Shader)
	{
		m_Shader = Shader;
		m_ConstantsDesc = m_Shader->GetBinder()->GetShaderConstantsDesc(ERootParameters::MaterialCB);
		m_ResourcesDesc = m_Shader->GetBinder()->GetShaderResourcesDesc();
		
		m_ConstantsBuffer = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(m_Device, *m_ConstantsDesc.get());
		m_ResourcesBuffer = FRenderer::GraphicFactroy->CreateShaderResourceBuffer(
			m_Device , 
			*m_ResourcesDesc.get(), 
			m_Shader->GetBinder()->GetRootSignature()
		);
	}

	void FMaterial::SetFloat(const std::string& Name, const float& Value)
	{
		m_ConstantsBuffer->SetFloat(Name, Value);
	}

	void FMaterial::SetFloat3(const std::string& Name, const ZMath::vec3& Value)
	{
		m_ConstantsBuffer->SetFloat3(Name, Value);
	}

	void FMaterial::SetFloat4(const std::string& Name, const ZMath::vec4& Value)
	{
		m_ConstantsBuffer->SetFloat4(Name, Value);
	}

	void FMaterial::SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value)
	{
		m_ConstantsBuffer->SetMatrix4x4(Name, Value);
	}

	void FMaterial::SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture)
	{
		m_ResourcesBuffer->SetTexture2D(Name, Texture);
	}

	void FMaterial::SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture)
	{
		m_ResourcesBuffer->SetTextureCubemap(Name, Texture);
	}

	void FMaterial::GetFloat(const std::string& Name, float& Value)
	{
		m_ConstantsBuffer->GetFloat(Name, Value);
	}

	void FMaterial::GetFloat3(const std::string& Name, ZMath::vec3& Value)
	{
		m_ConstantsBuffer->GetFloat3(Name, Value);
	}

	void FMaterial::GetFloat4(const std::string& Name, ZMath::vec4& Value)
	{
		m_ConstantsBuffer->GetFloat4(Name, Value);
	}

	void FMaterial::GetMatrix4x4(const std::string& Name, ZMath::mat4& Value)
	{
		m_ConstantsBuffer->GetMatrix4x4(Name, Value);
	}

	float* FMaterial::PtrFloat(const std::string& Name)
	{
		return m_ConstantsBuffer->PtrFloat(Name);
	}

	float* FMaterial::PtrFloat3(const std::string& Name)
	{
		return m_ConstantsBuffer->PtrFloat3(Name);
	}

	float* FMaterial::PtrFloat4(const std::string& Name)
	{
		return m_ConstantsBuffer->PtrFloat4(Name);
	}

	float* FMaterial::PtrMatrix4x4(const std::string& Name)
	{
		return m_ConstantsBuffer->PtrMatrix4x4(Name);
	}
	
}

#include "Material.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/Texture.h"
#include "World/World.h"
#include "World/Actor/CameraActor.h"
#include "Core/Framework/Library.h"
#include "Core/Framework/Application.h"
#include "Core/Base/FrameTimer.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{

	FMaterial::FMaterial(bool bUseMainCamera)
		: m_bUseMainCamera(bUseMainCamera)
	{	
		m_OnSetFloat.Bind<FMaterial>(this, &FMaterial::SetFloatPtr);
		m_OnSetInt.Bind<FMaterial>(this, &FMaterial::SetIntPtr);
		m_SetParameterMap.insert(
			{EShaderDataType::Float, m_OnSetFloat}
		);
		m_SetParameterMap.insert(
			{EShaderDataType::Int, m_OnSetInt}
		);
	}


	FMaterial::~FMaterial()
	{
		if (m_bUseMainCamera)
		{
			FConstantsBufferManager::Get().PushCameraConstantsBufferToPool(m_CameraBuffer);
		}
	}

	void FMaterial::Tick()
	{
	}

	void FMaterial::SetPass(FCommandListHandle CommandListHandle)
	{
		m_ShaderBinder->Bind(CommandListHandle);
		const auto& BinderDesc = m_ShaderBinder->GetBinderDesc();
		if (m_MaterialBuffer != nullptr)
		{
			m_MaterialBuffer->PreDrawCall();
			m_ShaderBinder->BindConstantsBuffer(CommandListHandle, BinderDesc.m_MaterialIndex, m_MaterialBuffer.get());
		}
		if (BinderDesc.m_CameraIndex != Utils::InvalidIndex)
		{
			if (m_bUseMainCamera)
			{
				UCameraActor* Camera = UWorld::GetCurrentWorld()->GetMainCamera();
				m_CameraBuffer = Camera->GetConstantBuffer();
			}
			else if (m_CameraBuffer == nullptr)
			{
				m_CameraBuffer = FConstantsBufferManager::Get().GetCameraConstantBuffer();
			}
			m_ShaderBinder->BindConstantsBuffer(CommandListHandle, BinderDesc.m_CameraIndex, m_CameraBuffer.get());
		}
		
		if (BinderDesc.m_GloabalConstantIndex != Utils::InvalidIndex)
		{
			m_ShaderBinder->BindConstantsBuffer(CommandListHandle, BinderDesc.m_GloabalConstantIndex, FConstantsBufferManager::Get().GetGlobalConstantBuffer().get());
		}
		//m_Shader->Use();
	}

	void FMaterial::OnDrawCall(FCommandListHandle CommandListHanle)
	{
		if (m_MaterialBuffer != nullptr)
		{
			m_MaterialBuffer->UploadDataIfDirty();
		}
		if (m_ResourcesBuffer != nullptr)
		{
			m_ResourcesBuffer->UploadDataIfDirty(CommandListHanle);
		}
		if (m_CameraBuffer != nullptr)
		{
			m_CameraBuffer->UploadDataIfDirty();
		}
		FConstantsBufferManager::Get().GetGlobalConstantBuffer()->UploadDataIfDirty();
	}

	void FMaterial::SetShader(Ref<FShader> Shader)
	{
		if (m_Shader != Shader)
		{
			m_Shader = Shader;
			m_ShaderBinder = m_Shader->GetBinder();
			int32_t MaterialIndex = Shader->GetBinder()->GetBinderDesc().m_MaterialIndex;
			if (MaterialIndex != Utils::InvalidIndex)
			{
				m_MaterialDesc = m_Shader->GetBinder()->GetShaderConstantsDesc(MaterialIndex);
				if (m_MaterialDesc != nullptr)
				{
					m_MaterialDesc->bDynamic = false;
					m_MaterialBuffer = FConstantsBufferManager::Get().GetMaterialConstBufferByDesc(m_MaterialDesc);
				}
			}
			m_ResourcesDesc = m_Shader->GetBinder()->GetShaderResourcesDesc();
			if (m_ResourcesDesc->Size > 0)
			{
				m_ResourcesBuffer = FRenderer::GraphicFactroy->CreateShaderResourceBuffer(
					*m_ResourcesDesc.get(),
					m_Shader->GetBinder()->GetRootSignature()
				);
			}
		}
	}

	void FMaterial::SetFloat(const std::string& Name, const float& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat(Name, Value);
	}

	void FMaterial::SetFloatPtr(const std::string& Name, void* Value)
	{
		if (m_MaterialBuffer)
			SetFloat(Name, *(float*)Value);
	}

	void FMaterial::SetInt(const std::string& Name, const int32_t& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetInt(Name, Value);
	}

	void FMaterial::SetIntPtr(const std::string& Name, void* ValueiPtr)
	{
		if (m_MaterialBuffer)
			SetInt(Name, *(int32_t*)ValueiPtr);
	}

	void FMaterial::SetFloat2(const std::string& Name, const ZMath::vec2& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat2(Name, Value);
	}

	void FMaterial::SetFloat3(const std::string& Name, const ZMath::vec3& Value)
	{
		if (m_MaterialBuffer)
		{
			m_MaterialBuffer->SetFloat3(Name, Value);
		}
	}

	void FMaterial::SetFloat4(const std::string& Name, const ZMath::vec4& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat4(Name, Value);
	}

	void FMaterial::SetFloat4Ptr(const std::string& Name, void* ValuePtr)
	{
		if (m_MaterialBuffer)
			SetFloat4(Name, *(ZMath::vec4*)ValuePtr);
	}

	void FMaterial::SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetMatrix4x4(Name, Value);
	}

	void FMaterial::SetCameraProjectMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FMaterial::SetCameraViewMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FMaterial::SetCameraProjectViewMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FMaterial::SetCameraViewPos(const std::string& Name, const ZMath::vec3& Value)
	{
		m_CameraBuffer->SetFloat3(Name, Value);
	}

	void FMaterial::SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture)
	{
		if (m_ResourcesBuffer)
		{
			m_ResourcesBuffer->SetTexture2D(Name, Texture);
		}
	}

	void FMaterial::SetTexture2DArray(const std::string& Name, const std::vector<Ref<FTexture2D>> Textures)
	{
		m_ResourcesBuffer->SetTexture2DArray(Name, Textures);
	}

	void FMaterial::SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture)
	{
		m_ResourcesBuffer->SetTextureCubemap(Name, Texture);
	}

	void FMaterial::SetTextureCubemapArray(const std::string& Name, const std::vector<Ref<FTextureCubemap>>& Textures)
	{
		m_ResourcesBuffer->SetTextureCubemapArray(Name, Textures);
	}

	void FMaterial::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr)
	{
		auto Iter = m_SetParameterMap.find(ShaderDataType);
		if (Iter != m_SetParameterMap.end())
		{
			Iter->second.Execute(ParameterName, ValuePtr);
		}
	}

	void FMaterial::GetFloat(const std::string& Name, float& Value)
	{
		m_MaterialBuffer->GetFloat(Name, Value);
	}

	void FMaterial::GetFloat3(const std::string& Name, ZMath::vec3& Value)
	{
		m_MaterialBuffer->GetFloat3(Name, Value);
	}

	void FMaterial::GetFloat4(const std::string& Name, ZMath::vec4& Value)
	{
		m_MaterialBuffer->GetFloat4(Name, Value);
	}

	void FMaterial::GetMatrix4x4(const std::string& Name, ZMath::mat4& Value)
	{
		m_MaterialBuffer->GetMatrix4x4(Name, Value);
	}

	float* FMaterial::PtrFloat(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat(Name);
	}

	float* FMaterial::PtrFloat3(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat3(Name);
	}

	float* FMaterial::PtrFloat4(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat4(Name);
	}

	float* FMaterial::PtrMatrix4x4(const std::string& Name)
	{
		return m_MaterialBuffer->PtrMatrix4x4(Name);
	}
	
}

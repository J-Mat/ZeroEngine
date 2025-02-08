#include "ShaderParamsSettings.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/Texture.h"
#include "Core/Framework/Library.h"
#include "Core/Framework/Application.h"
#include "Core/Base/FrameTimer.h"
#include "World/World.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{

	FShaderParamsSettings::FShaderParamsSettings()
	{	
		m_OnSetFloat.Bind<FShaderParamsSettings>(this, &FShaderParamsSettings::SetFloatPtr);
		m_OnSetInt.Bind<FShaderParamsSettings>(this, &FShaderParamsSettings::SetIntPtr);
		m_SetParameterMap.insert(
			{EShaderDataType::Float, m_OnSetFloat}
		);
		m_SetParameterMap.insert(
			{EShaderDataType::Int, m_OnSetInt}
		);
	}

	FShaderParamsSettings::~FShaderParamsSettings()
	{
	}

	void FShaderParamsSettings::Tick()
	{
	}

	void FShaderParamsSettings::SetCamera(Ref<IShaderConstantsBuffer> Camera)
	{
		if (Camera != m_CameraBuffer)
		{
			const auto& BinderDesc = m_ShaderBinder->GetBinderDesc();
			if (BinderDesc.m_CameraIndex != Utils::InvalidIndex)
			{
				m_CameraBuffer = Camera;
				m_ShaderBinder->BindConstantsBuffer(m_CommandListHandle, BinderDesc.m_CameraIndex, m_CameraBuffer.get());
			}
		}
	}

	void FShaderParamsSettings::SetPass(FCommandListHandle CommandListHandle, ERenderPassType RenderPassUsage)
	{
		m_CommandListHandle = CommandListHandle;
		m_ShaderBinder->Bind(CommandListHandle, RenderPassUsage);
		const auto& BinderDesc = m_ShaderBinder->GetBinderDesc();
		if (m_MaterialBuffer != nullptr)
		{
			m_MaterialBuffer->PreDrawCall();
			m_ShaderBinder->BindConstantsBuffer(CommandListHandle, BinderDesc.m_MaterialIndex, m_MaterialBuffer.get());
		}
		
		if (BinderDesc.m_CameraIndex != Utils::InvalidIndex)
		{
			m_CameraBuffer = UWorld::GetCurrentWorld()->GetMainCamera()->GetConstantBuffer();
			m_ShaderBinder->BindConstantsBuffer(m_CommandListHandle, BinderDesc.m_CameraIndex, m_CameraBuffer.get());
		}

		if (BinderDesc.m_GloabalConstantIndex != Utils::InvalidIndex)
		{
			m_ShaderBinder->BindConstantsBuffer(CommandListHandle, BinderDesc.m_GloabalConstantIndex, FConstantsBufferManager::Get().GetGlobalConstantBuffer().get());
		}
	}


	void FShaderParamsSettings::OnDrawCall(FCommandListHandle CommandListHanle, ERenderPassType RenderPassUsage)
	{
		if (m_ResourcesBuffer != nullptr)
		{
			m_ResourcesBuffer->UploadDataIfDirty(CommandListHanle, RenderPassUsage);
		}
	}

	void FShaderParamsSettings::PostDrawCall()
	{
		m_CommandListHandle = {};
	}

	void FShaderParamsSettings::SetShader(Ref<FShader> Shader)
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
					m_MaterialDesc->bDynamic = true;
					m_MaterialBuffer = FConstantsBufferManager::Get().GetMaterialConstBufferByDesc(m_MaterialDesc);
				}
			}
			m_ResourcesDesc = m_Shader->GetBinder()->GetShaderResourcesDesc();
			if (m_ResourcesDesc->Size > 0)
			{
				m_ResourcesBuffer = FGraphic::GraphicFactroy->CreateShaderResourceBuffer(
					*m_ResourcesDesc.get(),
					m_Shader->GetBinder()->GetRootSignature()
				);
			}
		}
	}

	void FShaderParamsSettings::SetFloat(const std::string& Name, const float& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat(Name, Value);
	}

	void FShaderParamsSettings::SetFloatPtr(const std::string& Name, void* Value)
	{
		if (m_MaterialBuffer)
			SetFloat(Name, *(float*)Value);
	}

	void FShaderParamsSettings::SetInt(const std::string& Name, const int32_t& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetInt(Name, Value);
	}

	void FShaderParamsSettings::SetIntPtr(const std::string& Name, void* ValueiPtr)
	{
		if (m_MaterialBuffer)
			SetInt(Name, *(int32_t*)ValueiPtr);
	}

	void FShaderParamsSettings::SetFloat2(const std::string& Name, const ZMath::vec2& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat2(Name, Value);
	}

	void FShaderParamsSettings::SetFloat3(const std::string& Name, const ZMath::vec3& Value)
	{
		if (m_MaterialBuffer)
		{
			m_MaterialBuffer->SetFloat3(Name, Value);
		}
	}

	void FShaderParamsSettings::SetFloat4(const std::string& Name, const ZMath::vec4& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetFloat4(Name, Value);
	}

	void FShaderParamsSettings::SetFloat4Ptr(const std::string& Name, void* ValuePtr)
	{
		if (m_MaterialBuffer)
			SetFloat4(Name, *(ZMath::vec4*)ValuePtr);
	}

	void FShaderParamsSettings::SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value)
	{
		if (m_MaterialBuffer)
			m_MaterialBuffer->SetMatrix4x4(Name, Value);
	}

	void FShaderParamsSettings::SetCameraProjectMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FShaderParamsSettings::SetCameraViewMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FShaderParamsSettings::SetCameraProjectViewMat(const std::string& Name, const ZMath::mat4& Value)
	{
		m_CameraBuffer->SetMatrix4x4(Name, Value);
	}

	void FShaderParamsSettings::SetCameraViewPos(const std::string& Name, const ZMath::vec3& Value)
	{
		m_CameraBuffer->SetFloat3(Name, Value);
	}

	void FShaderParamsSettings::SetTexture2D(const std::string& Name, FTexture2D* Texture)
	{
		if (m_ResourcesBuffer)
		{
			m_ResourcesBuffer->SetTexture2D(Name, Texture);
		}
	}

	void FShaderParamsSettings::SetTexture2D_Uav(const std::string& Name, FTexture2D* Texture)
	{
		if (m_ResourcesBuffer)
		{
			m_ResourcesBuffer->SetTexture2D_Uav(Name, Texture);
		}
	}

	void FShaderParamsSettings::SetBuffer(const std::string& Name, FBuffer* Buffer)
	{
		if (m_ResourcesBuffer)
		{
			m_ResourcesBuffer->SetBuffer(Name, Buffer);
		}
	}

	void FShaderParamsSettings::SetBuffer_Uav(const std::string& Name, FBuffer* Buffer)
	{
		if (m_ResourcesBuffer)
		{
			m_ResourcesBuffer->SetBuffer_Uav(Name, Buffer);
		}
	}

	void FShaderParamsSettings::SetTexture2DArray(const std::string& Name, const std::vector<FTexture2D*> Textures)
	{
		m_ResourcesBuffer->SetTexture2DArray(Name, Textures);
	}

	void FShaderParamsSettings::SetTextureCube(const std::string& Name, FTextureCube* Texture)
	{
		if (m_ResourcesBuffer != nullptr)
		{
			m_ResourcesBuffer->SetTextureCube(Name, Texture);
		}
	}

	void FShaderParamsSettings::SetTextureCubemapArray(const std::string& Name, const std::vector<Ref<FTextureCube>>& Textures)
	{
		m_ResourcesBuffer->SetTextureCubemapArray(Name, Textures);
	}

	void FShaderParamsSettings::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr)
	{
		auto Iter = m_SetParameterMap.find(ShaderDataType);
		if (Iter != m_SetParameterMap.end())
		{
			Iter->second.Execute(ParameterName, ValuePtr);
		}
	}

	void FShaderParamsSettings::GetFloat(const std::string& Name, float& Value)
	{
		m_MaterialBuffer->GetFloat(Name, Value);
	}

	void FShaderParamsSettings::GetFloat3(const std::string& Name, ZMath::vec3& Value)
	{
		m_MaterialBuffer->GetFloat3(Name, Value);
	}

	void FShaderParamsSettings::GetFloat4(const std::string& Name, ZMath::vec4& Value)
	{
		m_MaterialBuffer->GetFloat4(Name, Value);
	}

	void FShaderParamsSettings::GetMatrix4x4(const std::string& Name, ZMath::mat4& Value)
	{
		m_MaterialBuffer->GetMatrix4x4(Name, Value);
	}

	float* FShaderParamsSettings::PtrFloat(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat(Name);
	}

	float* FShaderParamsSettings::PtrFloat3(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat3(Name);
	}

	float* FShaderParamsSettings::PtrFloat4(const std::string& Name)
	{
		return m_MaterialBuffer->PtrFloat4(Name);
	}

	float* FShaderParamsSettings::PtrMatrix4x4(const std::string& Name)
	{
		return m_MaterialBuffer->PtrMatrix4x4(Name);
	}
	
}

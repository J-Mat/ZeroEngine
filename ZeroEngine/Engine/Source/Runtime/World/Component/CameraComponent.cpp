#include "CameraComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"

namespace Zero
{
	static FShaderConstantsDesc PerCameraConstantsDesc;
	static FShaderConstantsDesc* GetPerCameraConstantsDesc()
	{
		// Init if not Inited
		if (PerCameraConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerCameraConstants;
			int paraIndex = 0;

			PerCameraConstantsDesc.Size = Layout.GetStride();
			for (auto BufferElement : Layout)
			{
				PerCameraConstantsDesc.Mapper.InsertConstant(BufferElement, ERootParameters::CameraCB);
			}
		}

		return &PerCameraConstantsDesc;
	}

	void UCameraComponent::PostInit()
	{
		FCameraSettings Settings;
		SetCameraSettings(Settings);
		m_ShaderConstantsBuffer = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(*GetPerCameraConstantsDesc());
	}

	UCameraComponent::~UCameraComponent()
	{
	}
	
	void UCameraComponent::SetCameraSettings(FCameraSettings& Setting)
	{
		m_CameraSettings = Setting;
		UpdateCameraSettings();
	}

	void UCameraComponent::UpdateCameraSettings()
	{
		switch (m_CameraSettings.Cameratype)
		{
		case ECameraType::CT_PERSPECT:
			m_Projection = ZMath::perspectiveLH(
				ZMath::radians(m_CameraSettings.Fov),
				m_CameraSettings.Aspect,
				m_CameraSettings.Near, 
				m_CameraSettings.Far
			);
		case ECameraType::CT_ORI:
			break;
		}
	}

	void UCameraComponent::UpdateMat()
	{
		m_LookAt = m_Position + m_ForwardVector;
		m_View = ZMath::lookAtLH(m_Position, m_LookAt, m_UpVector);
		m_ProjectionView = m_Projection * m_View;
	}

	void UCameraComponent::UploadBuffer()
	{
		if (m_ShaderConstantsBuffer != nullptr)
		{
			m_ShaderConstantsBuffer->SetMatrix4x4("Projection", m_Projection);
			m_ShaderConstantsBuffer->SetMatrix4x4("View", m_View);
			m_ShaderConstantsBuffer->SetMatrix4x4("ProjectionView", m_ProjectionView);
			m_ShaderConstantsBuffer->SetFloat3("ViewPos", m_Position);
		}
		m_ShaderConstantsBuffer->UploadDataIfDirty();
	}

	void UCameraComponent::OnResizeViewport(uint32_t Width, uint32_t Height)
	{
		m_CameraSettings.Aspect = (float)Width / Height;
		UpdateCameraSettings();
	}

	void UCameraComponent::Tick()
	{
		UpdateMat(); 
		UploadBuffer();
	}
}

#include "CameraComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{
	UCameraComponent::UCameraComponent()
		: UComponent()
	{
	}

	UCameraComponent::~UCameraComponent()
	{
	}

	void UCameraComponent::PostInit()
	{
		m_TransformCompent =  static_cast<UTransformComponent*>(m_Parent);
		m_GlobalConstantsBuffer = FConstantsBufferManager::GetInstance().GetCameraConstantBuffer();
	}

	void UCameraComponent::UpdateCameraSettings()
	{
		switch (m_CameraType)
		{
		case ECameraType::CT_PERSPECT:
			m_Projection = ZMath::perspectiveLH(
				ZMath::radians(m_Fov),
				m_Aspect,
				m_Near, 
				m_Far
			);
			break;
		case ECameraType::CT_ORI:
			m_Projection = ZMath::orthoLH(
				-1000.0f,
				+1000.0f,
				-1000.0f,
				+1000.0f,
				m_Near, 
				m_Far
			);
			break;
		}
	}

	void UCameraComponent::UpdateMat()
	{
		m_LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_View = ZMath::lookAtLH(m_TransformCompent->m_Position, m_LookAt, m_TransformCompent->m_UpVector);
		m_ProjectionView = m_Projection * m_View;
	}

	void UCameraComponent::UploadBuffer()
	{
		if (m_GlobalConstantsBuffer != nullptr)
		{
			m_GlobalConstantsBuffer->SetMatrix4x4("Projection", m_Projection);
			m_GlobalConstantsBuffer->SetMatrix4x4("View", m_View);
			m_GlobalConstantsBuffer->SetMatrix4x4("ProjectionView", m_ProjectionView);
			m_GlobalConstantsBuffer->SetFloat3("ViewPos", m_TransformCompent->m_Position);
			m_GlobalConstantsBuffer->UploadDataIfDirty();
		}
	}

	void UCameraComponent::OnResizeViewport(uint32_t Width, uint32_t Height)
	{
		m_Aspect = (float)Width / Height;
		UpdateCameraSettings();
	}

	void UCameraComponent::Tick()
	{
		UpdateCameraSettings();
		UpdateMat(); 
		UploadBuffer();
	}
}

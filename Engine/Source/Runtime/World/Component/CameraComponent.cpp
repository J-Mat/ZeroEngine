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
		m_CameraBuffer = FConstantsBufferManager::Get().GetCameraConstantBuffer();
	}

	void UCameraComponent::UpdateCameraSettings()
	{
	}

	void UCameraComponent::UpdateMat()
	{
		m_LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_View = ZMath::lookAtLH(m_TransformCompent->m_Position, m_LookAt, m_TransformCompent->m_UpVector);
		m_ProjectionView = m_Projection * m_View;
	}

	void UCameraComponent::UploadBuffer()
	{
		if (m_CameraBuffer != nullptr)
		{
			m_CameraBuffer->SetMatrix4x4("Projection", m_Projection);
			m_CameraBuffer->SetMatrix4x4("View", m_View);
			m_CameraBuffer->SetMatrix4x4("ProjectionView", m_ProjectionView);
			m_CameraBuffer->SetFloat3("ViewPos", m_TransformCompent->m_Position);
			m_CameraBuffer->UploadDataIfDirty();
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

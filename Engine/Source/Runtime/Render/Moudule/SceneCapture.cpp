#include "SceneCapture.h"
#include "Render/Moudule/ConstantsBufferManager.h"
#include "Math/ZMath.h"

namespace Zero
{ 
	FSceneCapture2D::FSceneCapture2D()
	{
		m_CameraBuffer = FConstantsBufferManager::Get().GetCameraConstantBuffer();
	}

	void FSceneCapture2D::UpdateParams()
	{
		switch (m_SceneView.CameraType)
		{
		case ECameraType::CT_PERSPECT:
			m_SceneView.Proj = ZMath::perspectiveLH(
				ZMath::radians(m_SceneView.Fov),
				m_SceneView.Aspect,
				m_SceneView.Near,
				m_SceneView.Far
			);
			break;
		case ECameraType::CT_ORI:
			m_SceneView.Proj = ZMath::orthoLH_ZO(
				m_SceneView.Left,
				m_SceneView.Right,
				m_SceneView.Bottom,
				m_SceneView.Top,
				m_SceneView.Near, 
				m_SceneView.Far
			);
			break;
		}
		m_SceneView.View = ZMath::lookAtLH(m_SceneView.ViewPos, m_SceneView.Target, m_SceneView.UpDir);
		m_SceneView.ProjectionView = m_SceneView.Proj * m_SceneView.View;

		UploadBuffer();
	}

	void FSceneCapture2D::UploadBuffer()
	{
		m_CameraBuffer->SetMatrix4x4("Projection", m_SceneView.Proj);
		m_CameraBuffer->SetMatrix4x4("View", m_SceneView.View);
		m_CameraBuffer->SetMatrix4x4("ProjectionView", m_SceneView.ProjectionView);
		m_CameraBuffer->SetFloat3("ViewPos", m_SceneView.ViewPos);
		m_CameraBuffer->SetFloat("NearZ", m_SceneView.Near);
		m_CameraBuffer->SetFloat("FarZ", m_SceneView.Far);
	}

	FSceneCaptureCube::FSceneCaptureCube()
	{
		UpdateParams();
	}

	void FSceneCaptureCube::SetViewPos(ZMath::vec3 ViewPos)
	{
		m_ViewPos = ViewPos;
	}

	void FSceneCaptureCube::UpdateParams()
	{
		static ZMath::vec3 Targets[6] =
		{
			m_ViewPos + ZMath::vec3(1.0f,  0.0f,  0.0f), // +X 
			m_ViewPos + ZMath::vec3(-1.0f, 0.0f,  0.0f), // -X 
			m_ViewPos + ZMath::vec3(0.0f,  1.0f,  0.0f), // +Y 
			m_ViewPos + ZMath::vec3(0.0f,  -1.0f, 0.0f), // -Y 
			m_ViewPos + ZMath::vec3(0.0f,  0.0f,  1.0f), // +Z 
			m_ViewPos + ZMath::vec3(0.0f,  0.0f, -1.0f)  // -Z 
		};

		static ZMath::vec3 Ups[6] =
		{
			{0.0f, 1.0f, 0.0f},  // +X 
			{0.0f, 1.0f, 0.0f},  // -X 
			{0.0f, 0.0f, -1.0f}, // +Y 
			{0.0f, 0.0f, +1.0f}, // -Y 
			{0.0f, 1.0f, 0.0f},	 // +Z 
			{0.0f, 1.0f, 0.0f}	 // -Z 
		};


		for (int i = 0; i < 6; ++i)
		{
			m_SceneViews[i].ViewPos = m_ViewPos;
			m_SceneViews[i].View = ZMath::lookAtLH(m_ViewPos, Targets[i], Ups[i]);

			float Fov = ZMath::half_pi<float>();
			float AspectRatio = 1.0f; //Square
			m_SceneViews[i].Near = m_Near;
			m_SceneViews[i].Far = m_Far;

			m_SceneViews[i].Proj = ZMath::perspectiveLH(Fov, AspectRatio, m_SceneViews[i].Near, m_SceneViews[i].Far);

			m_SceneViews[i].ProjectionView = m_SceneViews[i].Proj * m_SceneViews[i].View;


			if (m_ViewCameraBuffers[i] == nullptr)
			{
				m_ViewCameraBuffers[i] = FConstantsBufferManager::Get().GetCameraConstantBuffer();
			}
		}
		UploadBuffer();
	}

	void FSceneCaptureCube::SetFarAndNear(float Far, float Near)
	{
		m_Far = Far;
		m_Near = Near;
	}

	void FSceneCaptureCube::UploadBuffer()
	{
		for (int i = 0; i < 6; ++i)
		{
			m_ViewCameraBuffers[i]->SetMatrix4x4("Projection", m_SceneViews[i].Proj);
			m_ViewCameraBuffers[i]->SetMatrix4x4("View", m_SceneViews[i].View);
			m_ViewCameraBuffers[i]->SetMatrix4x4("ProjectionView", m_SceneViews[i].ProjectionView);
			m_ViewCameraBuffers[i]->SetFloat3("ViewPos", m_SceneViews[i].ViewPos);
			m_ViewCameraBuffers[i]->SetFloat("NearZ", m_SceneViews[i].Near);
			m_ViewCameraBuffers[i]->SetFloat("FarZ", m_SceneViews[i].Far);
		}
	}

}


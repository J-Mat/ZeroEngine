#include "SceneCapture.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{ 
	FSceneCapture2D::FSceneCapture2D()
	{

	}
	void FSceneCapture2D::UpdateParams()
	{
		switch (m_SceneView.CameraType)
		{
		case ECameraType::CT_PERSPECT:
			m_SceneView.Proj = ZMath::perspectiveLH(
				ZMath::radians(m_SceneView.Proj),
				m_SceneView.Aspect,
				m_SceneView.Near,
				m_SceneView.Far
			);
			break;
		case ECameraType::CT_ORI:
			m_SceneView.Proj = ZMath::orthoLH(
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

	FSceneCaptureCube::FSceneCaptureCube()
	{
		UpdateParams();
		
	}

	void FSceneCaptureCube::SetViewPos(ZMath::vec3 ViewPos)
	{
		m_ViewPos = ViewPos;
		UpdateParams();
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
			m_SceneViews[i].Near = 0.01f;
			m_SceneViews[i].Far = 10.0f;

			m_SceneViews[i].Proj = ZMath::perspectiveLH(Fov, AspectRatio, m_SceneViews[i].Near, m_SceneViews[i].Far);

			m_SceneViews[i].ProjectionView = m_SceneViews[i].Proj * m_SceneViews[i].View;


			if (m_ViewCameraBuffers[i] == nullptr)
			{
				m_ViewCameraBuffers[i] = FConstantsBufferManager::Get().GetCameraConstantBuffer();
			}
		}
	}

	void FSceneCaptureCube::SetFarAndNear(float Far, float Near)
	{
		m_Far = Far;
		m_Near = Near;
		UpdateParams();
	}


}


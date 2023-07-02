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
	}


	void UCameraComponent::OnResizeViewport(uint32_t Width, uint32_t Height)
	{
		static 	FSceneView& SceneView = m_SceneCapture2D.GetSceneView();
		SceneView.Aspect = (float)Width / Height;
		SceneView.ScreenHeight = Width;
		SceneView.ScreenHeight = Height;
	}

	void UCameraComponent::Tick()
	{
		static 	FSceneView& SceneView = m_SceneCapture2D.GetSceneView();
		SceneView.ViewPos = m_TransformCompent->m_Position;
		SceneView.ForwardDir = m_TransformCompent->m_ForwardVector;
		SceneView.UpDir = m_TransformCompent->m_UpVector;
		SceneView.RightDir = m_TransformCompent->m_RightVector;
		SceneView.Target = SceneView.ViewPos + SceneView.ForwardDir;
		m_SceneCapture2D.UpdateParams();
	}
}

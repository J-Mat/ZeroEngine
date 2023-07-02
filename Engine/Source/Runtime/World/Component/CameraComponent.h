#pragma once
#include "Core.h"
#include "TransformComponent.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/Moudule/SceneCapture.h"
#include "CameraComponent.reflection.h"

namespace Zero
{

	UCLASS()
	class UCameraComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		UCameraComponent();
		virtual ~UCameraComponent();
		virtual void PostInit();
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_SceneCapture2D.GetCamera(); }
		const FSceneView& GetSceneView() const { return m_SceneCapture2D.GetSceneView(); }
		FSceneView& GetSceneView() { return m_SceneCapture2D.GetSceneView(); }
		void OnResizeViewport(uint32_t Width, uint32_t Height);
		virtual void Tick();

	private:
		UTransformComponent* m_TransformCompent;
		
		FSceneCapture2D m_SceneCapture2D;
	};
}
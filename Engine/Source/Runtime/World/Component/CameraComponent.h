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
		void UpdateCameraSettings();
		void UpdateMat();
		void UploadBuffer();
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_CameraBuffer; }
		void OnResizeViewport(uint32_t Width, uint32_t Height);
		virtual void Tick();
		ZMath::mat4 GetProjection() { return m_Projection;}
		ZMath::mat4 GetView() { return m_View; }

	private:
		UTransformComponent* m_TransformCompent;
		
		FSceneCapture2D m_SceneCapture2D;

		ZMath::vec3 m_LookAt = {0, 0, 0};
		ZMath::mat4 m_Projection;
		ZMath::mat4 m_ProjectionDither;
		ZMath::mat4 m_View;
		ZMath::mat4 m_ProjectionView;
		Ref<IShaderConstantsBuffer> m_CameraBuffer = nullptr;

		float m_Fov = 90.0f;

	};
}
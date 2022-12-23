#pragma once
#include "Core.h"
#include "TransformComponent.h"
#include "Render/RHI/ShaderBinder.h"
#include "CameraComponent.reflection.h"

namespace Zero
{
	UENUM()
	enum ECameraType
	{
		CT_PERSPECT,
		CT_ORI,
	};

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
		

		ZMath::vec3 m_LookAt = {0, 0, 0};
		ZMath::mat4 m_Projection;
		ZMath::mat4 m_ProjectionDither;
		ZMath::mat4 m_View;
		ZMath::mat4 m_ProjectionView;
		Ref<IShaderConstantsBuffer> m_CameraBuffer = nullptr;

		float m_Fov = 90.0f;

		float m_Aspect = 16.0f / 9.0f;
		ZMath::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };

		UPROPERTY()
		ECameraType m_CameraType = ECameraType::CT_PERSPECT;

		float m_Near = 0.01f;
		float m_Far = 1000.0f;
		float m_Left = 0.0f;
		float m_Right = 0.0f;
		float m_Bottom = 0.0f;
		float m_Top = 0.0f;
	};
}
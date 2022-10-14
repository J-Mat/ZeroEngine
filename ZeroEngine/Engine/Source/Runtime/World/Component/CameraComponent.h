#pragma once
#include "Core.h"
#include "TransformComponent.h"
#include "Render/RHI/ShaderBinder.h"

namespace Zero
{
	enum ECameraType
	{
		CT_PERSPECT,
		CT_ORI,
	};


	struct FCameraSettings
	{
		FCameraSettings() = default;
		float Fov = 90.0f;
		float Aspect = 16.0f / 9.0f;
		ZMath::vec3 WorldUp = { 0.0f, 1.0f, 0.0f };
		ECameraType Cameratype = ECameraType::CT_PERSPECT;

		float Near = 0.01f;
		float Far = 1000.0f;
		float Left = 0.0f;
		float Right = 0.0f;
		float Bottom = 0.0f;
		float Top = 0.0f;
	};

	class UCameraComponent : public UTransformationComponent
	{
	public:
		UCameraComponent();
		virtual ~UCameraComponent();
		void SetCameraSettings(FCameraSettings& Setting);
		void UpdateCameraSettings();
		void UpdateMat();
		void UploadBuffer();
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_ShaderConstantsBuffer; }
		void OnResizeViewport(uint32_t Width, uint32_t Height);
		virtual void Tick();
		ZMath::mat4 GetProjection() { return m_Projection;}
		ZMath::mat4 GetView() { return m_View; }
	private:
		ZMath::vec3 m_LookAt = {0, 0, 0};
		ZMath::mat4 m_Projection;
		ZMath::mat4 m_ProjectionDither;
		ZMath::mat4 m_View;
		ZMath::mat4 m_ProjectionView;
		FCameraSettings m_CameraSettings;
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
	};
}
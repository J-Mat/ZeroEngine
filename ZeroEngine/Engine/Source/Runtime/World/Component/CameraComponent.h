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
		float Fov = 60.0f;
		float Aspect = 4.0f / 3.0f;
		ZMath::vec3 WorldUp = { 0.0f, 1.0f, 0.0f };
		ECameraType Cameratype = ECameraType::CT_PERSPECT;

		float Near = 0.1f;
		float Far = 10000.0f;
		float Left = 0.0f;
		float Right = 0.0f;
		float Bottom = 0.0f;
		float Top = 0.0f;
	};

	class UCameraComponent : public UTransformationComponent
	{
	public:
		UCameraComponent() = default;
		virtual void PostInit();
		virtual ~UCameraComponent();
		void SetCameraSettings(FCameraSettings& Setting);
		void UpdateMat();
		void UploadBuffer();
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_ShaderConstantsBuffer; }
		virtual void Tick();
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
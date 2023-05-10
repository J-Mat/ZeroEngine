#pragma once

#include "Core.h"
#include "Render/RHI/Shader/ShaderBinder.h"

namespace Zero
{
	enum ECameraType
	{
		CT_PERSPECT,
		CT_ORI,
	};

	struct FSceneView
	{
		ZMath::vec3 ViewPos = ZMath::vec3(0.0f);
		ZMath::mat4 View = ZMath::identity<ZMath::mat4>();
		ZMath::mat4 Proj = ZMath::identity<ZMath::mat4>();
		ZMath::mat4 ProjectionView = ZMath::identity<ZMath::mat4>();
		float Near = 0.1f;
		float Far = 1000.0f;
		float Left = -1000.0f;
		float Right = +1000.0f;
		float Bottom = -1000.0f;
		float Top = +1000.0f;
		float Fov = 90.0f;

		float Aspect = 16.0f / 9.0f;
		ZMath::vec3 UpDir = { 0.0f, 1.0f, 0.0f };
		ZMath::vec3 Target = { 0.0f, 0.0f, 0.0f };
		ZMath::vec3 RightDir = { 1.0f, 0.0f, 0.0f };
		ZMath::vec3 ForwardDir = { 0.0f, 1.0f, 0.0f };
		ECameraType CameraType = CT_PERSPECT;
	}; 
	class FSceneCapture2D
	{
	public:
		FSceneCapture2D();
		const FSceneView& GetSceneView() const { return m_SceneView; }
		FSceneView& GetSceneView() { return m_SceneView; }
		Ref<IShaderConstantsBuffer> GetCamera() { return m_CameraBuffer;}
		void UpdateParams();
		void UploadBuffer();
	private:
		Ref<IShaderConstantsBuffer> m_CameraBuffer;
		FSceneView m_SceneView;
	};

	class FSceneCaptureCube
	{
	public:
		FSceneCaptureCube();
		const FSceneView& GetSceneView(uint32_t Index) const { return m_SceneViews[Index]; }
		Ref<IShaderConstantsBuffer> GetCamera(uint32_t Index) const {	return m_ViewCameraBuffers[Index];}
		void SetFarAndNear(float Far, float Near);
		void SetViewPos(ZMath::vec3 ViewPos);
		void UpdateParams();
		void UploadBuffer();
	private:
		ZMath::vec3 m_ViewPos = { 0.0f, 0.0f, 0.0f };
		float m_Near = 0.01f;
		float m_Far = 10.0f; 
		FSceneView m_SceneViews[6];
		Ref<IShaderConstantsBuffer> m_ViewCameraBuffers[6] = {};
	};
}
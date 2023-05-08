#pragma once

#include "Core.h"

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
		float Near;
		float Far;
		float Left = 0.0f;
		float Right = 0.0f;
		float Bottom = 0.0f;
		float Top = 0.0f;
		float Fov = 90.0f;

		float Aspect = 16.0f / 9.0f;
		ZMath::vec3 WorldUp = { 0.0f, 1.0f, 0.0f };
		ECameraType CameraType = CT_PERSPECT;
	}; 
	class FSceneCapture2D
	{
	public:
		FSceneCapture2D();
		const FSceneView& GetSceneView() const { return m_SceneView; }
	private:
		void UpdateParams();
	private:
		FSceneView m_SceneView;
	};

	class IShaderConstantsBuffer;
	class FSceneCaptureCube
	{
	public:
		FSceneCaptureCube();
		const FSceneView& GetSceneView(uint32_t Index) const { return m_SceneViews[Index]; }
		Ref<IShaderConstantsBuffer> GetCamera(uint32_t Index) const {	return m_ViewCameraBuffers[Index];}
		void SetFarAndNear(float Far, float Near);
		void SetViewPos(ZMath::vec3 ViewPos);
	private:
		void UpdateParams();
	private:
		ZMath::vec3 m_ViewPos = { 0.0f, 0.0f, 0.0f };
		float m_Near = 0.01f;
		float m_Far = 10.0f; 
		FSceneView m_SceneViews[6];
		Ref<IShaderConstantsBuffer> m_ViewCameraBuffers[6] = {};
	};
}
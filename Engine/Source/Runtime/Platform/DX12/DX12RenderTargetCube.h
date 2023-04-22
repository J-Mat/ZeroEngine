#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12Texture2D.h"
#include "Render/RHI/RenderTarget.h"


namespace Zero
{
	class FDX12Texture2D;
	class FDX12Device;
	class FDX12RenderTargetCube : public FRenderTargetCube
	{
	public:
		FDX12RenderTargetCube(const FRenderTargetCubeDesc& Desc);
		void SetViewportRect(uint32_t Mip);
		virtual void Bind(FCommandListHandle CommandListHandle) override;
		virtual void SetRenderTarget(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource = -1) override;
		virtual void UnBind(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource = -1) override;
	private:
		D3D12_VIEWPORT m_ViewPort = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		D3D12_RECT m_ScissorRect = { 0, 0, 0, 0 };
	};
}
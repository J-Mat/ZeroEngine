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
		void SetViewportRect();
		virtual void SetRenderTarget(uint32_t Index) override;
		virtual void Bind() override;
		virtual void UnBind() override;
	private:
		D3D12_VIEWPORT m_ViewPort = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		D3D12_RECT m_ScissorRect = { 0, 0, 0, 0 };
	};
}
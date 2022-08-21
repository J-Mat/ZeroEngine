#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/VertexBuffer.h"
#include "Base/Buffer.h"

namespace Zero
{
	class FDX12Device;
	class FDX1VertexBuffer :public IVertexBuffer, public IBuffer
	{
	public:
		FDX1VertexBuffer(FDX12Device& m_Device, void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type = IVertexBuffer::EType::Static);

	private:
		FDX12Device& m_Device;
		void CreateVertexBufferView();
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	};


}
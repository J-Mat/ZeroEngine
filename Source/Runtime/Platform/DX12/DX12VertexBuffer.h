#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/VertexBuffer.h"
#include "Base/Buffer.h"

namespace Zero
{
	class FDX12Device;
	class FDX12VertexBuffer :public IVertexBuffer, public IBuffer
	{
	public:
		FDX12VertexBuffer(void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type = IVertexBuffer::EType::Static);

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();
	private:

		void CreateVertexBufferView();
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	};


}
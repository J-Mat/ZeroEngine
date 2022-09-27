#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"
#include "Render/RHI/IndexBuffer.h"
#include "DX12Device.h"
#include "Base/Buffer.h"

namespace Zero
{
	class FDX12IndexBuffer :public IIndexBuffer, public IBuffer
	{
	public:
		FDX12IndexBuffer(uint32_t* Indices, uint32_t Count);
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	private:
		void CreateIndexBufferView();
	private:

		uint32_t IndexCount;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}
#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"
#include "Render/RHI/IndexBuffer.h"
#include "DX12Device.h"

namespace Zero
{
	class FDX12IndexBuffer :public FIndexBuffer
	{
	public:
		FDX12IndexBuffer(unsigned int* Indices, uint32_t Count);
		void SetData(unsigned int* Indices, UINT32 Count);
		virtual uint32_t Count() override { return IndexCount; }
		//const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	private:
		uint32_t IndexCount;
	};
}
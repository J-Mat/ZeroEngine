#include "DX12IndexBuffer.h"

namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(FDX12Device& Device, uint32_t* Indices, uint32_t Count)
		: IIndexBuffer(Indices, Count)
		, IBuffer(Device)
		, m_Device(Device)
	{	
		SetResource(m_Device.CreateDefaultBuffer(m_Data, m_BufferSize));
		CreateIndexBufferView();
	}

	void FDX12IndexBuffer::CreateIndexBufferView()
	{
		m_IndexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_BufferSize;
	}
}
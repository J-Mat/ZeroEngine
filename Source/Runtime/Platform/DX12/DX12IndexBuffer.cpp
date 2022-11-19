#include "DX12IndexBuffer.h"
#include "DX12Device.h"
#include "DX12CommandList.h"


namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(uint32_t* Indices, uint32_t Count)
		: IIndexBuffer(Indices, Count)
		, IBuffer()
	{	
		ComPtr<ID3D12Resource> Resource = FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(m_Data, m_BufferSize);
		SetResource(Resource);
		CreateIndexBufferView();
	}

	void FDX12IndexBuffer::CreateIndexBufferView()
	{
		m_IndexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_BufferSize;
	}
}
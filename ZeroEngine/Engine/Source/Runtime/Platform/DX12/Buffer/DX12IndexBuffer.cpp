#include "DX12IndexBuffer.h"
#include "../DX12Device.h"
#include "../DX12CommandList.h"
#include "../Resource.h"


namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(void* Indices, const FBufferDesc& Desc)
		: FIndexBuffer(Indices, Desc)
		, FResource()
	{
		ComPtr<ID3D12Resource> Resource = FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(m_Data, m_Desc.Size);
		SetResource(Resource);
		CreateIndexBufferView();
	}

	void FDX12IndexBuffer::CreateIndexBufferView()
	{
		m_IndexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_IndexBufferView.Format = FDX12Utils::ConvertResourceFormat(m_Desc.Format);// DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_Desc.Size;
	}
}
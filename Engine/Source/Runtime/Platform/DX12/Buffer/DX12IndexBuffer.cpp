#include "DX12IndexBuffer.h"
#include "../DX12Device.h"
#include "../DX12CommandList.h"
#include "../MemoryManage/Resource/Resource.h"


namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(void* Indices, const FBufferDesc& Desc)
		: FIndexBuffer(Indices, Desc)
	{
		//m_D3DResource = FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(m_Data, m_Desc.Size);
		FDX12Device::Get()->GetInitWorldCommandList()->CreateAndInitDefaultBuffer(
			m_Data,
			m_Desc.Size,
			DEFAULT_RESOURCE_ALIGNMENT,
			m_ResourceLocation);
		CreateIndexBufferView();
	}

	void FDX12IndexBuffer::CreateIndexBufferView()
	{
		m_IndexBufferView.BufferLocation = m_ResourceLocation.m_GPUVirtualAddress;
		m_IndexBufferView.Format = FDX12Utils::ConvertResourceFormat(m_Desc.Format);// DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_Desc.Size;
	}
}
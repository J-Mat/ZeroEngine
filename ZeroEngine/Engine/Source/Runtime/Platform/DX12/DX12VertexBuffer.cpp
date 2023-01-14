#include "DX12VertexBuffer.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12VertexBuffer::FDX12VertexBuffer(void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type)
		:IVertexBuffer(_Data, _VertexCount, _Layout, _Type)
		, FBuffer()
	{
		SetResource(FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(m_Data, m_BufferSize));
		CreateVertexBufferView();
	}
	
	const D3D12_VERTEX_BUFFER_VIEW& FDX12VertexBuffer::GetVertexBufferView()
	{
		return m_VertexBufferView;
	}

	void FDX12VertexBuffer::CreateVertexBufferView()
	{
		m_VertexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_BufferSize);
		m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_Layout.GetStride());
	}
}

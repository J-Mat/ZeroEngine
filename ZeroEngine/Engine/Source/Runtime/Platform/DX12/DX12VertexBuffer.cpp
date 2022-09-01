#include "DX12VertexBuffer.h"
#include "DX12Device.h"

namespace Zero
{
	FDX1VertexBuffer::FDX1VertexBuffer(FDX12Device& _Device, void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type)
		:IVertexBuffer(_Data, _VertexCount, _Layout, _Type)
		, IBuffer(_Device)
		, m_Device(_Device)
	{
		SetResource(m_Device.CreateDefaultBuffer(m_Data, m_BufferSize));
		CreateVertexBufferView();
	}
	
	const D3D12_VERTEX_BUFFER_VIEW& FDX1VertexBuffer::GetVertexBufferView()
	{
		return m_VertexBufferView;
	}

	void FDX1VertexBuffer::CreateVertexBufferView()
	{
		m_VertexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_VertexCount * m_Layout.GetStride());
		m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_Layout.GetStride());
	}
}

#include "DX12VertexBuffer.h"
#include "DX12Device.h"

namespace Zero
{
	FDX1VertexBuffer::FDX1VertexBuffer(FDX12Device& _Device, void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type)
		:IVertexBuffer(_Device, _Data, _VertexCount, _Layout, _Type)
		, IBuffer(_Device)
		, m_Device(_Device)
	{
		CreateVertexBufferView();
		//m_Device.GetCommandQueue().GetCommandList()->CreateDefaultBuffer()
	}
	void FDX1VertexBuffer::CreateVertexBufferView()
	{
		m_VertexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_VertexCount * m_Layout.GetStride());
		m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_Layout.GetStride());
	}
}

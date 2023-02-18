#include "DX12VertexBuffer.h"
#include "../DX12Device.h"
#include "../DX12CommandList.h"

namespace Zero
{
	FDX12VertexBuffer::FDX12VertexBuffer(void* Data, const FBufferDesc& Desc, FVertexBufferLayout& _Layout)
		:FVertexBuffer(Data, Desc, _Layout),
		FDX12Resource()
	{
		SetResource(FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(m_Data, m_Desc.Size));
		CreateVertexBufferView();
	}

	const D3D12_VERTEX_BUFFER_VIEW& FDX12VertexBuffer::GetVertexBufferView()
	{
		return m_VertexBufferView;
	}

	void FDX12VertexBuffer::CreateVertexBufferView()
	{
		m_VertexBufferView.BufferLocation = m_D3DResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_Desc.Size);
		m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_Layout.GetStride());
	}
}

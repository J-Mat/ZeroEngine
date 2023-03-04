#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Platform/DX12/MemoryManage/Resource/Resource.h"

namespace Zero
{
	class FDX12Device;
	class FDX12VertexBuffer :public FVertexBuffer
	{
	public:
		FDX12VertexBuffer(void* _Data, const FBufferDesc& Desc, FVertexBufferLayout& _Layout);

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();
	private:
		ComPtr<ID3D12Resource> m_D3DResource;
		FResourceLocation m_ResourceLocation;

		void CreateVertexBufferView();
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	};


}
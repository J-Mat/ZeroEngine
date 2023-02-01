#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/Buffer/VertexBuffer.h"

namespace Zero
{
	class FDX12Device;
	class FResource;
	class FDX12VertexBuffer :public FVertexBuffer, public FResource
	{
	public:
		FDX12VertexBuffer(void* _Data, const FBufferDesc& Desc, FVertexBufferLayout& _Layout);

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();
	private:

		void CreateVertexBufferView();
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	};


}
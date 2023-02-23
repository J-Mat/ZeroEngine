#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/Buffer/IndexBuffer.h"
#include "../DX12Device.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "Platform/DX12/MemoryManage/Resource/Resource.h"

namespace Zero
{
	class FDX12IndexBuffer :public FIndexBuffer
	{
	public:
		FDX12IndexBuffer(void* Indices, const FBufferDesc& Desc);
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	private:
		void CreateIndexBufferView();
	private:
		FResourceLocation m_ResourceLocation;

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}
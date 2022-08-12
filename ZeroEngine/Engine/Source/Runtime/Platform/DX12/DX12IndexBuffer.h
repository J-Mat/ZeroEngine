#pragma once
#include "Core.h"
#include "Render/RHI/IndexBuffer.h"

namespace Zero
{
	class FDX12IndexBuffer :public FIndexBuffer
	{
	public:
		FDX12IndexBuffer(unsigned int* Indices, uint32_t Count);
		void SetData(unsigned int* Indices, UINT32 Count);
		virtual uint32_t Count() override { return IndexCount; }
		//const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	private:
		uint32_t IndexCount;
		//Ref<DX12Resource> m_Resource;
		//D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};


}
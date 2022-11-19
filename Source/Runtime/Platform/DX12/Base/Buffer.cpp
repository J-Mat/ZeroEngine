#include "Buffer.h"

namespace Zero
{
	IBuffer::IBuffer()
		:IResource()
	{
	}
	IBuffer::IBuffer(const D3D12_RESOURCE_DESC& ResDesc)
		:IResource(ResDesc)
	{
	}

	IBuffer::IBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> Resource)
		:IResource(Resource)
	{
	}
}

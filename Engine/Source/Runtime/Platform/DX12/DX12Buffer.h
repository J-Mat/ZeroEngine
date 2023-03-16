#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "./MemoryManage/Resource/Resource.h"

namespace Zero
{
	class FDX12Buffer : public FBuffer
	{
	public:
		FDX12Buffer(const FBufferDesc& Desc, void* Data = 0);
	private:
		FResourceLocation m_ResourceLocation;
	};
}
#pragma once

#include "Core.h"
#include "../../Common/DX12Header.h"

namespace Zero
{
	struct FDynamicAllocation
	{
		void* CpuAddress = nullptr;
		ComPtr<ID3D12Resource> UploadBuffer = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS GpuAddress = 0;
		size_t Offset = 0;
		size_t Size = 0;

		void Update(void const* Data, size_t Size, size_t Offset = 0)
		{
			memcpy((uint8_t*)CpuAddress + Offset, Data, Size);
		}

		template<typename T>
		void Update(T const& Data)
		{
			memcpy(CpuAddress, &Data, sizeof(T));
		}
	};
}
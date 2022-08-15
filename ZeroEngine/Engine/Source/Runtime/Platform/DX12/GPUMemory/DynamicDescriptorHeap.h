#pragma once

/**
 *  @file DynamicDescriptorHeap.h
 *  @date October 22, 2018
 *  @author Jeremiah van Oosten
 *
 *  @brief The DynamicDescriptorHeap is a GPU visible descriptor heap that allows for
 *  staging of CPU visible descriptors that need to be uploaded before a Draw
 *  or Dispatch command is executed.
 *  The DynamicDescriptorHeap class is based on the one provided by the MiniEngine:
 *  https://github.com/Microsoft/DirectX-Graphics-Samples
 */
#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"

namespace Zero
{
	class FDynamicDescriptorHeap
	{
	public:
		FDynamicDescriptorHeap(FDX12Device& Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t HumDescriptorsPerHeap = 1024);
		
		virtual ~FDynamicDescriptorHeap();

		/**
		* Parse the root signature to determine which root parameters contain
		* descriptor tables and determine the number of descriptors needed for
		* each table.
		*/
		void ParseRootSignature(const std::shared_ptr<RootSignature>& rootSignature);
		
	private:
		/**
		* The maximum number of descriptor tables per root signature.
		* A 32-bit mask is used to keep track of the root parameter indices that
		* are descriptor tables.
		*/
		static const uint32_t s_MaxDescriptorTables = 32;


		/**
		* A structure that represents a descriptor table entry in the root signature.
		*/
		struct FDescriptorTableCache
		{
			FDescriptorTableCache()
				: NumDescriptors(0)
				, BaseDescriptor(nullptr)
			{}
			
			void Reset()
			{
				NumDescriptors = 0;
				BaseDescriptor = nullptr;
			}

			// The number of descriptors in this descriptor table.
			uint32_t NumDescriptors;
			// The pointer to the descriptor in the descriptor handle cache.
			D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
		};

		FDX12Device& m_Device;

		// Describes the type of descriptors that can be staged using this
		// dynamic descriptor heap.
		// Valid values are:
		//   * D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		//   * D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		// This parameter also determines the type of GPU visible descriptor heap to
		// create.
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;


		// The number of descriptors to allocate in new GPU visible descriptor heaps.
		uint32_t m_NumDescriptorsPerHeap;

		// The increment size of a descriptor.
		uint32_t m_DescriptorHandleIncrementSize;

		// The descriptor handle cache.
		Scope<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_DescriptorHandleCache;

		// Descriptor handle cache per descriptor table.
		FDescriptorTableCache m_DescriptorTableCache[s_MaxDescriptorTables];
		
		// Inline CBV
		D3D12_GPU_VIRTUAL_ADDRESS m_InlineCBV[s_MaxDescriptorTables];
		// Inline SRV
		D3D12_GPU_VIRTUAL_ADDRESS m_InlineSRV[s_MaxDescriptorTables];
		// Inline UAV
		D3D12_GPU_VIRTUAL_ADDRESS m_InlineUAV[s_MaxDescriptorTables];


		// Each bit in the bit mask represents the index in the root signature
		// that contains a descriptor table.
		uint32_t m_DescriptorTableBitMask;
		// Each bit set in the bit mask represents a descriptor table
		// in the root signature that has changed since the last time the
		// descriptors were copied.
		uint32_t	m_StaleDescriptorTableBitMask;
		uint32_t	m_StaleCBVBitMask;
		uint32_t	m_StaleSRVBitMask;
		uint32_t	m_StaleUAVBitMask;

		using FDescriptorHeapPool = std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>;
		

		FDescriptorHeapPool m_DescriptorHeapPool;
		FDescriptorHeapPool m_AvailableDescriptorHeaps;
		
		ComPtr<ID3D12DescriptorHeap>	m_CurrentDescriptorHeap;
		CD3DX12_GPU_DESCRIPTOR_HANDLE	m_CurrentGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE	m_CurrentCPUDescriptorHandle;

		uint32_t m_NumFreeHandles;
	};
}


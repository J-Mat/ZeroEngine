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

#include "Core.h"
#include "../Common/DX12Header.h"
#include "../DX12RootSignature.h"

namespace Zero
{
	class FDX12Device;
	class FDX12CommandList;
	class FDynamicDescriptorHeap
	{
	public:
		FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t HumDescriptorsPerHeap = 128);
		
		virtual ~FDynamicDescriptorHeap();

		/**
		* Stages a contiguous range of CPU visible descriptors.
		* Descriptors are not copied to the GPU visible descriptor heap until
		* the CommitStagedDescriptors function is called.
		*/
		void StageDescriptors(uint32_t RootParameterIndex, uint32_t Offset, uint32_t NumDescriptors,
			const D3D12_CPU_DESCRIPTOR_HANDLE SrcDescriptors);


		void SetAsShaderResourceHeap();


		/**
		 * Stage an inline CBV descriptor.
		 */
		void StageInlineCBV(uint32_t RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);

		/**
		 * Stage an inline SRV descriptor.
		 */
		void StageInlineSRV(uint32_t RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);

		/**
		 * Stage an inline UAV descriptor.
		 */
		void StageInlineUAV(uint32_t RootParamterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);


		/**
		* Parse the root signature to determine which root parameters contain
		* descriptor tables and determine the number of descriptors needed for
		* each table.
		*/
		void ParseRootSignature(const Ref<FDX12RootSignature>& RootSignature);

		void CommitStagedDescriptorsForDraw(FDX12CommandList& CommandList);

		void Reset();
		
	private:
		// Request a descriptor heap if one is available.
		ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
		// Create a new descriptor hea p of no descriptor heap is available.
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

		// Compute the number of stale descriptors that need to be copied
		// to GPU visible descriptor heap.
		uint32_t ComputeStaleDescriptorCount() const;

		/**
		* Copy all of the staged descriptors to the GPU visible descriptor heap and
		* bind the descriptor heap and the descriptor tables to the command list.
		* The passed-in function object is used to set the GPU visible descriptors
		* on the command list. Two possible functions are:
		*   * Before a draw    : ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable
		*   * Before a dispatch: ID3D12GraphicsCommandList::SetComputeRootDescriptorTable
		*
		* Since the DynamicDescriptorHeap can't know which function will be used, it must
		* be passed as an argument to the function.
		*/
		void CommitDescriptorTables(
			FDX12CommandList& CommandList,
			std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> SetFunc);
		void CommitInlineDescriptors(
			FDX12CommandList& CommandList, const D3D12_GPU_VIRTUAL_ADDRESS* bufferLocations, uint32_t& BitMask,
			std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> SetFunc);

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
				, m_BaseDescriptor(nullptr)
			{}
			
			void Reset()
			{
				NumDescriptors = 0;
				m_BaseDescriptor = nullptr;
			}

			// The number of descriptors in this descriptor table.
			uint32_t NumDescriptors;
			// The pointer to the descriptor in the descriptor handle cache.
			D3D12_CPU_DESCRIPTOR_HANDLE* m_BaseDescriptor;
		};



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
		CD3DX12_GPU_DESCRIPTOR_HANDLE	m_RecordGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE	m_CurrentCPUDescriptorHandle;

		uint32_t m_NumFreeHandles;
	};
}


#include "DX12ResourceBarrierBatch.h"
#include "DX12Texture2D.h"
#include "DX12Buffer.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{

	FDX12ResourceBarrierBatch::FDX12ResourceBarrierBatch(Ref<FDX12CommandList> CommandList):
		m_CommandList(CommandList)
	{
	}

	void FDX12ResourceBarrierBatch::AddTransition(void* Resource, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Transition = {
				.pResource = (ID3D12Resource*)Resource,
				.Subresource = SubResource,
				.StateBefore = FDX12Utils::ConvertToD3DResourceState(StateBefore),
				.StateAfter = FDX12Utils::ConvertToD3DResourceState(StateAfter),
			}
		};
		m_ResourceBarriers.push_back(ResourceBarrier);
	}


	void FDX12ResourceBarrierBatch::AddUAV(void* Resource)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV,
			.UAV = {
				.pResource = (ID3D12Resource*)Resource,
			}
		};
		m_ResourceBarriers.push_back(ResourceBarrier);
	}

	void FDX12ResourceBarrierBatch::AddAliasing(void* Before, void* After)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier =
		{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING,
			.Aliasing = {
				.pResourceBefore = (ID3D12Resource*)Before,
				.pResourceAfter = (ID3D12Resource*)After,
			},
		};
		m_ResourceBarriers.push_back(ResourceBarrier);
	}

	void FDX12ResourceBarrierBatch::Submit()
	{
		if (!m_ResourceBarriers.empty())
		{
			m_CommandList->GetD3D12CommandList()->ResourceBarrier(static_cast<UINT>(m_ResourceBarriers.size()), m_ResourceBarriers.data());
		}
	}

	void FDX12ResourceBarrierBatch::ReverseTransitions()
	{
		for (auto& ResourceBarrier : m_ResourceBarriers)
		{
			if (ResourceBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
				std::swap(ResourceBarrier.Transition.StateBefore, ResourceBarrier.Transition.StateAfter);
		}
	}

	void FDX12ResourceBarrierBatch::Clear()
	{
		m_ResourceBarriers.clear();
	}

}
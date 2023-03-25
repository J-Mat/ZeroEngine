#include "DX12ResourceBarrierBatch.h"
#include "DX12Texture2D.h"
#include "DX12Buffer.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	void FDX12ResourceBarrierBatch::AddTransition(Ref<FTexture2D> Texture, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{
		//AddTransition((ID3D12Resource*)Texture->GetNative(), StateBefore, StateAfter, SubResource);
	}

	void FDX12ResourceBarrierBatch::AddTransition(Ref<FBuffer> Buffer, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{
		//AddTransition((ID3D12Resource*)Buffer->GetNative(), StateBefore, StateAfter, SubResource);
	}

	void FDX12ResourceBarrierBatch::AddTransition(ID3D12Resource* Resource, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Transition = {
				.pResource = Resource,
				.Subresource = SubResource,
				.StateBefore = FDX12Utils::ConvertToD3DResourceState(StateBefore),
				.StateAfter = FDX12Utils::ConvertToD3DResourceState(StateAfter),
			}
		};
		m_ResourceBarriers.push_back(ResourceBarrier);
	}

	void FDX12ResourceBarrierBatch::AddUAV(Ref<FTexture2D> Texture)
	{
		auto* DX12Texture = static_cast<FDX12Texture2D*>(Texture.get());
		//AddUAV((ID3D12Resource*)Texture->GetNative());
	}

	void FDX12ResourceBarrierBatch::AddUAV(Ref<FBuffer> Buffer)
	{
		//AddUAV((ID3D12Resource*)Buffer->GetNative());
	}

	void FDX12ResourceBarrierBatch::AddUAV(ID3D12Resource* Resource)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV,
			.UAV = {
				.pResource = Resource,
			}
		};
		m_ResourceBarriers.push_back(ResourceBarrier);
	}

	void FDX12ResourceBarrierBatch::AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After)
	{
		//AddAliasing((ID3D12Resource*)Before->GetNative(), (ID3D12Resource*)After->GetNative());
	}

	void FDX12ResourceBarrierBatch::AddAliasing(Ref<FBuffer> Before, Ref<FBuffer> After)
	{
		//AddAliasing((ID3D12Resource*)Before->GetNative(), (ID3D12Resource*)After->GetNative());
	}


	void FDX12ResourceBarrierBatch::AddAliasing(ID3D12Resource* Before,ID3D12Resource* After)
	{
		D3D12_RESOURCE_BARRIER resource_barrier =
		{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING,
			.Aliasing = {
				.pResourceBefore = Before,
				.pResourceAfter = After,
			},
		};
		m_ResourceBarriers.push_back(resource_barrier);
	}

	void FDX12ResourceBarrierBatch::Submit(FCommandListHandle CommandListHandle)
	{
		if (!m_ResourceBarriers.empty())
		{
			auto CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
			CommandList->GetD3D12CommandList()->ResourceBarrier(static_cast<UINT>(m_ResourceBarriers.size()), m_ResourceBarriers.data());
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
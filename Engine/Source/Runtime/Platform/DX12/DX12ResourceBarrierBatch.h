#pragma once 
#include "Core.h"
#include "Render/RHI/ResourceBarrierBatch.h"
#include "Common/DX12Header.h"

namespace Zero
{
	class FDX12CommandList;
	class FDX12ResourceBarrierBatch : public FResourceBarrierBatch
	{
	public:
		FDX12ResourceBarrierBatch(Ref<FDX12CommandList> CommandList);
		virtual void AddTransition(void* Resource, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) override;
		virtual void AddUAV(void* Resource) override;
		virtual void AddAliasing(void* Before, void* After) override;
		virtual void Submit() override;
		virtual void ReverseTransitions() override;
		virtual void Clear() override;
	private:
		Ref<FDX12CommandList> m_CommandList;
		std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;
	};
}
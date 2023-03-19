#pragma once 
#include "Core.h"
#include "Render/RHI/ResourceBarrierBatch.h"
#include "Common/DX12Header.h"

namespace Zero
{
	class FDX12ResourceBarrierBatch : public FResourceBarrierBatch
	{
	public:
		FDX12ResourceBarrierBatch() = default;
		virtual void AddTransition(Ref<FTexture2D>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) override;
		virtual void AddTransition(Ref<FBuffer>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) override;
		virtual void AddUAV(Ref<FTexture2D>) override;
		virtual void AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After) override;
		virtual void Submit(FCommandListHandle CommandListHandle) override;
	private:
		std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;
	};
}
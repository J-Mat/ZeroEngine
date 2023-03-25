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
		void AddTransition(ID3D12Resource* Resource, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/);
		virtual void AddTransition(Ref<FBuffer>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) override;
		virtual void AddUAV(Ref<FTexture2D> Texture) override;
		virtual void AddUAV(Ref<FBuffer> Buffer) override;
		void AddUAV(ID3D12Resource* Resource);
		virtual void AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After) override;
		virtual void AddAliasing(Ref<FBuffer> Before, Ref<FBuffer> After) override;
		void AddAliasing(ID3D12Resource* Before, ID3D12Resource* After);
		virtual void Submit(FCommandListHandle CommandListHandle) override;
		virtual void ReverseTransitions() override;
		virtual void Clear() override;
	private:
		std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;
	};
}
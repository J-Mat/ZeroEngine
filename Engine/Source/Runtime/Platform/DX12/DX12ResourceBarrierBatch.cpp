#include "DX12ResourceBarrierBatch.h"
#include "DX12Texture2D.h"

namespace Zero
{
	void FDX12ResourceBarrierBatch::AddTransition(Ref<FTexture2D>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{

	}

	void FDX12ResourceBarrierBatch::AddTransition(Ref<FBuffer>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource /*= -1*/)
	{

	}

	void FDX12ResourceBarrierBatch::AddUAV(Ref<FTexture2D>)
	{

	}

	void FDX12ResourceBarrierBatch::AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After)
	{

	}

	void FDX12ResourceBarrierBatch::Submit(FCommandListHandle CommandListHandle)
	{

	}

}


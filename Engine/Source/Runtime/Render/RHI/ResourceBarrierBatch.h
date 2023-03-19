#pragma once
#include "Core.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	class FTexture2D;
	class FBuffer;
	class FResourceBarrierBatch
	{
	public:
		FResourceBarrierBatch() = default;
		virtual void AddTransition(Ref<FTexture2D>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) = 0;
		virtual void AddTransition(Ref<FBuffer>, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) = 0;
		virtual void AddUAV(Ref<FTexture2D>) = 0;
		virtual void AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After) = 0;
		virtual void Submit(FCommandListHandle CommandListHandle) = 0;
	};
}
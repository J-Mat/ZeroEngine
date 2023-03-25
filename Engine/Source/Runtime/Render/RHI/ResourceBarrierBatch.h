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
		virtual void AddUAV(Ref<FTexture2D> Texture) = 0;
		virtual void AddUAV(Ref<FBuffer> Buffer) = 0;
		virtual void AddAliasing(Ref<FTexture2D> Before, Ref<FTexture2D> After) = 0;
		virtual void AddAliasing(Ref<FBuffer> Before, Ref<FBuffer> After) = 0;
		virtual void Submit(FCommandListHandle CommandListHandle) = 0;
		virtual void ReverseTransitions() = 0;
		virtual void Clear() = 0;
	};
}
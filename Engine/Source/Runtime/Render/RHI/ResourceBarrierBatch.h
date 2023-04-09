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
		virtual void AddTransition(void* Resource, EResourceState StateBefore, EResourceState StateAfter, uint32_t SubResource = -1) = 0;
		virtual void AddUAV(void* Resource) = 0;
		virtual void AddAliasing(void* Before, void* After) = 0;
		virtual void Submit() = 0;
		virtual void ReverseTransitions() = 0;
		virtual void Clear() = 0;
	};
}
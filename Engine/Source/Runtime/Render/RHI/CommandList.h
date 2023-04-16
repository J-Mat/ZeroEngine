#pragma once
#include "Core.h"

namespace Zero
{
	class FCommandList 
	{
	public:
		virtual ~FCommandList() {
		
		}
		virtual void Reset() = 0;
		virtual void Execute() = 0;
		virtual void SetComandListHandle(FCommandListHandle Handle) { m_CommandListHandle = Handle; }
		virtual void CopyResource(void* Dst, void* Src) = 0;
		virtual void TransitionBarrier(void* Resource, EResourceState ResourceState, UINT Subresource = -1, bool bFlushBarriers = false) = 0;
	protected:
		FCommandListHandle m_CommandListHandle = -1;
	};
}
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
	protected:
		FCommandListHandle m_CommandListHandle = -1;
	};
}
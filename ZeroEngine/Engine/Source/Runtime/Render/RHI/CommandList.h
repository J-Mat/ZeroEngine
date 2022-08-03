#pragma once

namespace Zero
{
	class FCommandList
	{
	public:
		virtual ~FCommandList() {
		
		}
		virtual void Reset() = 0;
		virtual void Execute() = 0;
	};
}
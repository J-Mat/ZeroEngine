#pragma once

#include "Core/Base/FrameTimer.h"

namespace Zero
{
	class FWindowsFrameTimer :public FFrameTimer
	{
	public:
		FWindowsFrameTimer();

		virtual void Reset() override;
		virtual void Start() override;
		virtual void Stop() override;
		virtual void Tick() override;

	private:

	};
}
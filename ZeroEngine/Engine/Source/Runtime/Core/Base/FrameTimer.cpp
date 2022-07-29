#include "FrameTimer.h"

#include "Platform/Windows/WinFrameTimer.h"

namespace Zero
{
	FFrameTimer* FFrameTimer::Create()
	{
		// Currently Only Support Windows
		return new FWindowsFrameTimer();
	}

	FFrameTimer::FFrameTimer():
		SencondsPerCount(0.0), DeltaTime(-1.0), BaseTime(0),
		PauseTime(0), StopTime(0), PrevTime(0), CurrentTime(0), bIsStoped(false)
	{

	}

	float FFrameTimer::GetTotalTime() const
	{
		if (bIsStoped)
		{
			return (float)((StopTime - PauseTime - BaseTime) * SencondsPerCount);
		}
		else
		{
			return (float)((CurrentTime - PauseTime - BaseTime) * SencondsPerCount);
		}
	}

	void FFrameTimer::RefreshFPS()
	{
		static float timeElapsed = 0.0f;
		FramePerSecond++;

		if (GetTotalTime() - timeElapsed >= 1.0f)
		{
			FPS = (float)FramePerSecond;
			MsPF = 1000.0f / FPS;
			FramePerSecond = 0;
			timeElapsed += 1.0f;
		}
	}
}
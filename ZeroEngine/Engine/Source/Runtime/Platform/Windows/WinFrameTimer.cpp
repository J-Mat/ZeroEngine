#include "Core.h"
#include "WinFrameTimer.h"

namespace Zero
{
	FWindowsFrameTimer::FWindowsFrameTimer()
	{
		// Initialize SecondPerCount according to system
		__int64 CountsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&CountsPerSec);
		SencondsPerCount = 1.0 / (double)CountsPerSec;
	}

	void FWindowsFrameTimer::Reset()
	{
		__int64 CurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&CurrTime);

		BaseTime = CurrTime;
		PrevTime = CurrTime;
		StopTime = 0;
		bIsStoped = false;
	}

	void FWindowsFrameTimer::Start()
	{
		__int64 StartTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);
		if (bIsStoped)
		{
			PauseTime += (StartTime - StopTime);
			PrevTime = StartTime;
			StopTime = 0;
			bIsStoped = false;
		}
	}

	void FWindowsFrameTimer::Stop()
	{
		if (!bIsStoped)
		{
			__int64 CurrTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&CurrTime);
			StopTime = CurrTime;
			bIsStoped = true;
		}
	}

	void FWindowsFrameTimer::Tick()
	{
		if (bIsStoped)
		{
			// If it's stopped now ...
			DeltaTime = 0.0;
			return;
		}

		// Calc the current time
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		CurrentTime = currentTime;
		DeltaTime = (CurrentTime - PrevTime) * SencondsPerCount;
		PrevTime = CurrentTime;
		if (DeltaTime < 0)
		{
			DeltaTime = 0;
		}

		// Recalculate FPS
		RefreshFPS();
	}
}
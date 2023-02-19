#include "Core.h"
#include "WinFrameTimer.h"

namespace Zero
{
	FWindowsFrameTimer::FWindowsFrameTimer()
	{
		// Initialize SecondPerCount according to system
		__int64 CountsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&CountsPerSec);
		m_SencondsPerCount = 1.0 / (double)CountsPerSec;
	}

	void FWindowsFrameTimer::Reset()
	{
		__int64 CurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&CurrTime);

		m_BaseTime = CurrTime;
		m_PrevTime = CurrTime;
		m_StopTime = 0;
		m_bIsStoped = false;
	}

	void FWindowsFrameTimer::Start()
	{
		__int64 StartTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);
		if (m_bIsStoped)
		{
			m_PauseTime += (StartTime - m_StopTime);
			m_PrevTime = StartTime;
			m_StopTime = 0;
			m_bIsStoped = false;
		}
	}

	void FWindowsFrameTimer::Stop()
	{
		if (!m_bIsStoped)
		{
			__int64 CurrTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&CurrTime);
			m_StopTime = CurrTime;
			m_bIsStoped = true;
		}
	}

	void FWindowsFrameTimer::Tick()
	{
		if (m_bIsStoped)
		{
			// If it's stopped now ...
			m_DeltaTime = 0.0;
			return;
		}

		// Calc the current time
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		m_CurrentTime = currentTime;
		m_DeltaTime = (m_CurrentTime - m_PrevTime) * m_SencondsPerCount;
		m_PrevTime = m_CurrentTime;
		if (m_DeltaTime < 0)
		{
			m_DeltaTime = 0;
		}

		// Recalculate FPS
		RefreshFPS();
	}
}
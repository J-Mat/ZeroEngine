#pragma once

#include <chrono>

namespace ZeroEngine
{
	class FTimer
	{
	public:
		FTimer()
		{
			Reset();
		}
		void Reset()
		{
			Start = std::chrono::high_resolution_clock::now();
		}
		float Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Start).count() * 0.001f * 0.001f * 0.001f;
		}
		float ElapsedMillis()
		{
			return Elapsed() * 1000.0f;
		}
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> Start;
	};


	class FFrameTimer
    {
    public:
        FTimestep(float InTime = 0.0f)
            : Time(InTime)
        {
        }

        operator float() const { return Time; }

    	 float GetSeconds() const { return Time; }
    	 float GetMilliseconds() const { return Time * 1000.0f; }
		
		float FrameTimer::TotalTime() const
		{
			if (m_IsStoped)
			{
				return (float)((StopTime - PauseTime - BaseTime) * m_SencondsPerCount);
			}
			else
			{
				return std::chrono::duration_cast<std::chrono::nanoseconds>(CurrentTime - PauseTime - BaseTime).count() * 0.001f * 0.001f * 0.001f;
			}
		}

		void FrameTimer::RefreshFPS()
		{
			static float TimeElapsed = 0.0f;
			FramePerSecond++;

		if (TotalTime() - TimeElapsed >= 1.0f)
		{
			FPS = (float)FramePerSecond;
			m_MsPF = 1000.0f / m_FPS;
			FramePerSecond = 0;
			TimeElapsed += 1.0f;
		}

		void Tick()
		{
			auto CurrentTime = std::chrono::high_resolution_clock::now();
			DeltaTime = CurrentTime - PrevTime;
			PrevTime = CurrentTime;
			if (DeltaTime < 0.0f)
			{
				DeltaTime = 0.0f;
			}

			RefreshFPS();
		}
    private:
		int FramePerSecond = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> BaseTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> PrevTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> StopTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> DeltaTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> PauseTime;
    };
} // namespace ZeroGame

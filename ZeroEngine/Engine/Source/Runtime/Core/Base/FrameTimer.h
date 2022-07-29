#pragma once

namespace Zero
{
	class FFrameTimer
	{
	public:
		static FFrameTimer* Create();
		FFrameTimer();

		float GetTotalTime()const;
		float GetDeltaTime()const { return (float)DeltaTime; }
		bool IsStoped() { return bIsStoped; }

		virtual void Reset() = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Tick() = 0;

		inline const float& GetFPS() const { return FPS; }
		inline const float& GetMsPF() const { return MsPF; }

	protected:
		void	RefreshFPS();
		__int64 FrameCount = 0;
		int		FramePerSecond = 0;
		float	FPS = 0;
		float	MsPF = 0;

	protected:
		double SencondsPerCount;
		double DeltaTime;

		__int64 BaseTime;
		__int64 PauseTime;	
		__int64 StopTime;
		__int64 PrevTime;
		__int64 CurrentTime;

		bool bIsStoped;
	};
}
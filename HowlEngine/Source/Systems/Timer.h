#pragma once
#include "chrono"

namespace HEngine
{
	class Timer
	{
	public:
		Timer();

		void Start();
		void Stop();
		void Reset();
		void Tick();

		double GetTotalTime() const;

		inline double GetDeltaTime() const { return mDeltaTime; };
	private:
		using clock = std::chrono::high_resolution_clock;
		using timepoint = std::chrono::time_point<clock>;

		timepoint mStartTime;
		timepoint mPauseTime;
		timepoint mPrevFrameTime;
		timepoint mCurrentFrameTime;

		bool mIsPaused = false;
		double mDeltaTime = 0.0;
	};
}



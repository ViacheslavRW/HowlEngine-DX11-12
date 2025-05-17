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

		float GetTotalTime() const;

		inline float GetDeltaTime() const { return mDeltaTime; };
	private:
		using clock = std::chrono::high_resolution_clock;
		using timepoint = std::chrono::time_point<clock>;

		timepoint mStartTime;
		timepoint mPauseTime;
		timepoint mPrevFrameTime;
		timepoint mCurrentFrameTime;

		bool mIsPaused = false;
		float mDeltaTime = 0.0f;
	};
}



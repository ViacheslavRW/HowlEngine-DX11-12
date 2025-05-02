#include "pch.h"
#include "Timer.h"

namespace HEngine
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Start()
	{
		if (mIsPaused)
		{
			auto startAgain = clock::now();
			mStartTime += (startAgain - mPauseTime);
			mPrevFrameTime = startAgain;
			mIsPaused = false;
		}
	}

	void Timer::Stop()
	{
		if (!mIsPaused)
		{
			mPauseTime = clock::now();
			mIsPaused = true;
		}
	}

	void Timer::Reset()
	{
		mCurrentFrameTime = clock::now();
		mPrevFrameTime = mCurrentFrameTime;
		mStartTime = mCurrentFrameTime;

		mIsPaused = false;
		mDeltaTime = 0.0;
	}

	void Timer::Tick()
	{
		if (mIsPaused)
		{
			mDeltaTime = 0.0;
			return;
		}

		mCurrentFrameTime = clock::now();
		mDeltaTime = std::chrono::duration<double>(mCurrentFrameTime - mPrevFrameTime).count();
		mPrevFrameTime = mCurrentFrameTime;

		if (mDeltaTime < 0.0) mDeltaTime = 0.0;
	}

	double Timer::GetTotalTime() const
	{
		if (mIsPaused)
		{
			return std::chrono::duration<double>(mPauseTime - mStartTime).count();
		}
		else
		{
			return std::chrono::duration<double>(mCurrentFrameTime - mStartTime).count();
		}
	}
}

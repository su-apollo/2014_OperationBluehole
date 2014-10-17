#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::OnTick()
{
	mNowTime = timeGetTime();

	if (mPrevTime == 0)
		mPrevTime = mNowTime;

	mDeltaTime = (static_cast<float>(mNowTime - mPrevTime)) / 1000.0f;

	mPrevTime = mNowTime;

	CountFPS();
}

void Timer::CountFPS()
{
	mFrameCounter++;

	mFPSTimer += mDeltaTime;

	if (mFPSTimer > 0.1f)
	{
		mFPS = (static_cast<float>(mFrameCounter)) / mFPSTimer;
		mFrameCounter = 0;
		mFPSTimer = 0.0f;
	}
}

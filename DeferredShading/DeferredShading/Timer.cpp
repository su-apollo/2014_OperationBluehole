#include "stdafx.h"
#include "Timer.h"
#include "Logger.h"
#include "PostProcessor.h"

Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::OnTick()
{
	CountDeltaTime();
	CountFPS();
}

void Timer::CountDeltaTime()
{
	mNowTime = timeGetTime();

	if (mPrevTime == 0)
		mPrevTime = mNowTime;

	mDeltaTime = (static_cast<float>(mNowTime - mPrevTime)) / 1000.0f;

	mPrevTime = mNowTime;
}

void Timer::CountFPS()
{
	mFrameCounter++;

	mFPSTimer += mDeltaTime;

	if (mFPSTimer > 1.0f)
	{
		mFPS = (static_cast<float>(mFrameCounter)) / mFPSTimer;
		mFrameCounter = 0;
		mFPSTimer = 0.0f;

		float r = PostProcessor::GetInstance()->GetKernelRadius();
		Log("DT : %f\n", mDeltaTime);
		Log("FPS : %f\n", mFPS);
		Log("Radius : %f\n", r);
	}
}



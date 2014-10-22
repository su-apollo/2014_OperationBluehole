#pragma once
#include "Singleton.h"




class Timer : public Singleton<Timer>
{
public:
	Timer();
	~Timer();

	void OnTick();

	float GetDeltaTime() { return mDeltaTime; }
	float GetFPS() { return mFPS; }

private:

	void CountFPS();
	void CountDeltaTime();

	UINT mPrevTime = 0;
	UINT mNowTime = 0;

	float mDeltaTime = 0;

	float mFPSTimer = 0;
	float mFPS = 0;

	UINT mFrameCounter = 0;


};


#pragma once
#include "Singleton.h"

enum {
	MAX_LIGHT = 2,
	KERNEL_NUM = 8
};

class Contents : public Singleton<Contents>
{
public:
	Contents();
	~Contents();

	void Init();
	void Update();

	int mCurrentMouseX = 0;
	int mCurrentMouseY = 0;
	int dx = 0, dy = 0;
	float mCameraX = 0.0f;
	float mCameraY = 0.0f;

	float mCurrentTime = 0.0f;
	float mLightRadius = 50.f;
};


#pragma once
#include "Singleton.h"

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

	float mAngle = 0.0f;
	float mLightRadius = 50.f;
	float mLightSpeed = 1.0f;
};


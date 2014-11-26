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
	void Render();

	int mCurrentMouseX = 0;
	int mCurrentMouseY = 0;
};


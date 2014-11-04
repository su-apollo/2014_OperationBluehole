#pragma once
#include "Singleton.h"

enum {
	MAX_LIGHT = 2
};

class Contents : public Singleton<Contents>
{
public:
	Contents();
	~Contents();


	void Init();
	void Render();

};


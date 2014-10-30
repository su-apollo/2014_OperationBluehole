#pragma once
#include "Singleton.h"





class Contents : public Singleton<Contents>
{
public:
	Contents();
	~Contents();


	void Init();
	void Render();




};


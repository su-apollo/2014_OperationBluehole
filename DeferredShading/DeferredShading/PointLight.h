#pragma once
#include "Billboard.h"




class PointLight
{
public:
	PointLight() : mPos(15.f, 60.0f, -50.0f, 1.0f), mRange(100), mFullRange(65), mColor(0.8f, 0.8f, 0.8f, 0.0f) 
	{
	}
	~PointLight() {}

	D3DXVECTOR4 mPos;
	D3DXVECTOR4 mColor;
	float		mRange;
	float		mFullRange;
};


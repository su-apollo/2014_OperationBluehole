#pragma once





class PointLight
{
public:
	PointLight() : mPos(10.f, 70.0f, -20.0f, 1.0f), mRange(100.0f), mColor(1.0f, 1.0f, 1.0f, 1.0f) {}
	~PointLight() {}


	D3DXVECTOR4 mPos;
	D3DXVECTOR4 mColor;
	float		mRange;


};


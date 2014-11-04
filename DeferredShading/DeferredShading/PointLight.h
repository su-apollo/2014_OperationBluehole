#pragma once





class PointLight
{
public:
	PointLight() : mPos(15.f, 60.0f, -50.0f, 1.0f), mRange(200), mColor(1.0f, 1.0f, 1.0f, 1.0f) {}
	~PointLight() {}


	D3DXVECTOR4 mPos;
	D3DXVECTOR4 mColor;
	float		mRange;


};


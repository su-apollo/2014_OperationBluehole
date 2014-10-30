#pragma once





class PointLight
{
public:
	PointLight() : mPos(0.1f, 0.0f, -0.1f, 1.0f), mRange(0.2f), mColor(1.0f, 1.0f, 1.0f, 1.0f) {}
	~PointLight() {}


	D3DXVECTOR4 mPos;
	D3DXVECTOR4 mColor;
	float		mRange;


};


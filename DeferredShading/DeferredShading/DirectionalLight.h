#pragma once





class DirectionalLight
{
public:

	DirectionalLight()
		: mDirection(1.0f, 0.0f, -1.0f, 1.0f), mColor(1.0f, 1.0f, 1.0f, 1.0f) {}
	~DirectionalLight() {}




	D3DXVECTOR4 mDirection;
	D3DXVECTOR4 mColor;

};


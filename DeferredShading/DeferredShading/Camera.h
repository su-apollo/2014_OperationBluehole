#pragma once
#include "Singleton.h"





class Camera : public Singleton<Camera>
{
public:
	Camera()
		: mEye(0.0f, 50.0f, -80.0f), mAt(0.0f, 0.0f, 1.0f), mUp(0.0f, 1.0f, 0.0f) {};
	~Camera() {}

	void		Update();
	D3DXVECTOR3	GetPosition() { return mEye; }

	D3DXMATRIX	GetMatView() { return mMatView; }
	D3DXMATRIX	GetMatProj() { return mMatProj; }

private:

	D3DXVECTOR3 mEye;
	D3DXVECTOR3 mAt;
	D3DXVECTOR3 mUp;

	float		mNear = 0.01f;
	float		mFar = 100.0f;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;
};


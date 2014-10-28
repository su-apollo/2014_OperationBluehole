#pragma once
#include "Singleton.h"

enum {
	FRUSTUM_NEAR = 10,
	FRUSTUM_FAR = 100
};


class Camera : public Singleton<Camera>
{
public:
	Camera()
		: mEye(0.0f, 50.0f, -50.0f), mAt(0.0f, 20.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f) {};
	~Camera() {}

	void		Update();

	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }

	D3DXVECTOR3	GetPosition() { return mEye; }

	D3DXMATRIX	GetMatView() { return mMatView; }
	D3DXMATRIX	GetMatProj() { return mMatProj; }
	D3DXMATRIX	GetMatInverseProj();

private:

	D3DXVECTOR3 mEye;
	D3DXVECTOR3 mAt;
	D3DXVECTOR3 mUp;

	float		mNear = FRUSTUM_NEAR;
	float		mFar = FRUSTUM_FAR;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;
};


#pragma once
#include "Singleton.h"

enum {
	FRUSTUM_NEAR = 100,
	FRUSTUM_FAR = 300
};

class Camera : public Singleton<Camera>
{
public:
	Camera();
	~Camera() {}

	void		Update();
	void		MoveFront(float speed);

	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }

	D3DXVECTOR3	GetPosition() { return mEye; }

	D3DXMATRIX	GetMatView() { return mMatView; }
	D3DXMATRIX	GetMatProj() { return mMatProj; }
	D3DXMATRIX	GetMatInverseProj();
	D3DXMATRIX	GetMatInverseViewProj();

private:

	D3DXVECTOR3 mEye;
	D3DXVECTOR3 mAt;
	D3DXVECTOR3 mUp;

	D3DXMATRIX	mTransform;

	float		mNear = FRUSTUM_NEAR;
	float		mFar = FRUSTUM_FAR;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;
};


#pragma once
#include "Singleton.h"

enum {
	FRUSTUM_NEAR = 10,
	FRUSTUM_FAR = 600
};

class Camera : public Singleton<Camera>
{
public:
	Camera();
	~Camera() {}

	void		Update();
	void		Walk(float speed);
	void		Strafe(float speed);
	void		Pitch(float angle);
	void		RotateY(float angle);
	void		changeNearFar(float range);

	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }

	D3DXVECTOR3	GetPosition() { return mEye; }

	D3DXMATRIX	GetMatView() { return mMatView; }
	D3DXMATRIX	GetMatProj() { return mMatProj; }
	D3DXMATRIX	GetMatInverseProj();
	D3DXMATRIX	GetMatInverseViewProj();
	void	UpdateMatView();

private:

	D3DXVECTOR3 mEye;
	D3DXVECTOR3 mAt;
	D3DXVECTOR3 mUp;

	D3DXMATRIX	mTransform;

	float		mNear = FRUSTUM_NEAR;
	float		mFar = FRUSTUM_FAR;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;

	//바꿀게요
	D3DXVECTOR3 mPosition; // 카메라 워점
	D3DXVECTOR3 mRight; //x
	D3DXVECTOR3 mEup; //y
	D3DXVECTOR3 mLook; //z

};


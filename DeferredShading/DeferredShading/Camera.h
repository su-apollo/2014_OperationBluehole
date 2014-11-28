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

	void		Strafe(float speed); // right vec
	void		Fly(float speed); // up vec
	void		Walk(float speed); // look vec

	void		Pitch(float angle);
	void		Yaw(float angle);
	void		Roll(float angle);

	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }

	D3DXVECTOR3	GetPosition() { return mPosition; }

	D3DXMATRIX	GetMatView() { return mMatView; }
	D3DXMATRIX	GetMatProj() { return mMatProj; }
	D3DXMATRIX	GetMatInverseProj();
	D3DXMATRIX	GetMatInverseViewProj();

private:

	void		UpdateViewMatrix();
	void		UpdateProjectionMatrix();

	D3DXVECTOR3 mPosition; // 카메라 워점
	D3DXVECTOR3 mRight; //x
	D3DXVECTOR3 mUp; //y
	D3DXVECTOR3 mLook; //z

	D3DXVECTOR3 mRealRight;
	D3DXVECTOR3 mRealUp;
	D3DXVECTOR3 mRealLook;

	float		mNear = FRUSTUM_NEAR;
	float		mFar = FRUSTUM_FAR;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;
};


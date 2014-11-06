#include "stdafx.h"
#include "Camera.h"
#include "Timer.h"
#include "App.h"


Camera::Camera()
: mPosition(0.0f, 100.0f, -150.0f), mRight(1.0f, 0.0f, 0.0f),mUp(0.0f, 1.0f, 0.0f),mLook(0.0f, 0.0f, 1.0f)
{
	D3DXMatrixIdentity(&mTransform);
}

void Camera::Update()
{
	RECT rc;
	GetClientRect(App::GetInstance()->GetHandleMainWindow(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	/*
	D3DXVECTOR3 eye, at, up;
	D3DXVec3TransformCoord(&eye, &mEye, &mTransform);
	D3DXVec3TransformCoord(&at, &mAt, &mTransform);
	D3DXVec3TransformCoord(&up, &mUp, &mTransform);

	D3DXMatrixLookAtLH(&mMatView, &eye, &at, &up);
	*/
	UpdateMatView();
	D3DXMatrixPerspectiveFovLH(&mMatProj, D3DX_PI / 4, width / (FLOAT)height, mNear, mFar);
}

D3DXMATRIX Camera::GetMatInverseProj()
{
	D3DXMATRIX output;
	D3DXMatrixInverse(&output, NULL, &mMatProj);
	return output;
}

D3DXMATRIX Camera::GetMatInverseViewProj()
{
	D3DXMATRIX output;
	D3DXMatrixMultiply(&output, &mMatView, &mMatProj);
	D3DXMatrixInverse(&output, NULL, &output);
	return output;
}

void Camera::Walk(float speed)
{
	float step = speed*Timer::GetInstance()->GetDeltaTime();
	D3DXVECTOR3 s = { step, step, step };
	D3DXVECTOR3 l = mLook;
	D3DXVECTOR3 p = mPosition;
	mPosition.x = s.x*l.x + p.x;
	mPosition.y = s.y*l.y + p.y;
	mPosition.z = s.z*l.z + p.z;

}

void Camera::Strafe(float speed)
{
	float step = speed*Timer::GetInstance()->GetDeltaTime();
	D3DXVECTOR3 s = { step, step, step };
	D3DXVECTOR3 r = mRight;
	D3DXVECTOR3 p = mPosition;
	mPosition.x = s.x*r.x + p.x;
	mPosition.y = s.y*r.y + p.y;
	mPosition.z = s.z*r.z + p.z;
}

void Camera::Pitch(float angle) // 고개 끄덕이는거.
{
	D3DXMATRIX R;
	D3DXMatrixRotationX(&R, Timer::GetInstance()->GetDeltaTime()*angle);
	D3DXVec3TransformCoord(&mUp, &mUp, &R);
	D3DXVec3TransformCoord(&mLook, &mLook, &R);
}

void Camera::RotateY(float angle) // 고개 도리도리
{
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Timer::GetInstance()->GetDeltaTime()*angle);
	D3DXVec3TransformCoord(&mRight, &mRight, &R);
	D3DXVec3TransformCoord(&mLook, &mLook, &R);
}

void Camera::UpdateMatView()
{
	//직교행렬을 새롭게 만든다!!
	D3DXVec3Normalize(&mLook, &mLook);
	D3DXVec3Cross(&mUp, &mLook, &mRight);
	D3DXVec3Normalize(&mUp, &mUp);
	D3DXVec3Cross(&mRight, &mUp, &mLook);
	D3DXVec3Normalize(&mRight, &mRight);

	//뷰 매트릭스를 만든다!
	float x = -D3DXVec3Dot(&mRight, &mPosition);
	float y = -D3DXVec3Dot(&mUp, &mPosition);
	float z = -D3DXVec3Dot(&mLook, &mPosition);

	mMatView(0, 0) = mRight.x; 
	mMatView(1, 0) = mRight.y;
	mMatView(2, 0) = mRight.z;
	mMatView(3, 0) = x;

	mMatView(0, 1) = mUp.x;
	mMatView(1, 1) = mUp.y;
	mMatView(2, 1) = mUp.z;
	mMatView(3, 1) = y;

	mMatView(0, 2) = mLook.x; 
	mMatView(1, 2) = mLook.y;
	mMatView(2, 2) = mLook.z;
	mMatView(3, 2) = z;

	mMatView(0, 3) = 0.0f;
	mMatView(1, 3) = 0.0f;
	mMatView(2, 3) = 0.0f;
	mMatView(3, 3) = 1.0f;
}

void Camera::changeNearFar(float range)
{
	mNear += range*Timer::GetInstance()->GetDeltaTime();
	mFar += range*Timer::GetInstance()->GetDeltaTime();
}



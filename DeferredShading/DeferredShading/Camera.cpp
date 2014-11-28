#include "stdafx.h"
#include "Camera.h"
#include "Timer.h"
#include "App.h"


Camera::Camera()
	: mPosition(0.0f, 100.0f, -150.0f), mRight(1.0f, 0.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f), mLook(0.0f, 0.0f, 1.0f), mRotation(0.0f, 0.0f, 0.0f)
{
}

void Camera::Update()
{
	mRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	Yaw(mRotation.x);
	Pitch(mRotation.y);
	Roll(mRotation.z);
	UpdateViewMatrix();
	UpdateProjectionMatrix();
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

void Camera::Strafe(float speed)
{
	float step = speed*Timer::GetInstance()->GetDeltaTime();
	mPosition += mRight * step;
}

void Camera::Fly(float speed)
{
	float step = speed*Timer::GetInstance()->GetDeltaTime();
	mPosition += mUp * step;
}

void Camera::Walk(float speed)
{
	float step = speed*Timer::GetInstance()->GetDeltaTime();
	mPosition += mLook * step;
}

void Camera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &mRight, angle);

	D3DXVec3TransformCoord(&mUp, &mUp, &T);
	D3DXVec3TransformCoord(&mLook, &mLook, &T);
}

void Camera::Yaw(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &mUp, angle);

	D3DXVec3TransformCoord(&mRight, &mRight, &T);
	D3DXVec3TransformCoord(&mLook, &mLook, &T);
}

void Camera::Roll(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &mLook, angle);

	D3DXVec3TransformCoord(&mRight, &mRight, &T);
	D3DXVec3TransformCoord(&mUp, &mUp, &T);
}

void Camera::UpdateViewMatrix()
{
	D3DXVec3Normalize(&mLook, &mLook);
	
	D3DXVec3Cross(&mUp, &mLook, &mRight);
	D3DXVec3Normalize(&mUp, &mUp);

	D3DXVec3Cross(&mRight, &mUp, &mLook);
	D3DXVec3Normalize(&mRight, &mRight);

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

void Camera::UpdateProjectionMatrix()
{
	RECT rc;
	GetClientRect(App::GetInstance()->GetHandleMainWindow(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3DXMatrixPerspectiveFovLH(&mMatProj, D3DX_PI / 4, width / (FLOAT)height, mNear, mFar);
}








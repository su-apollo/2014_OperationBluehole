#include "stdafx.h"
#include "Camera.h"
#include "Timer.h"
#include "App.h"


Camera::Camera()
: mEye(0.0f, 100.0f, -150.0f), mAt(0.0f, 50.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f)
{
	D3DXMatrixIdentity(&mTransform);
}

void Camera::Update()
{
	RECT rc;
	GetClientRect(App::GetInstance()->GetHandleMainWindow(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3DXVECTOR3 eye, at, up;
	D3DXVec3TransformCoord(&eye, &mEye, &mTransform);
	D3DXVec3TransformCoord(&at, &mAt, &mTransform);
	D3DXVec3TransformCoord(&up, &mUp, &mTransform);

	D3DXMatrixLookAtLH(&mMatView, &eye, &at, &up);
	D3DXMatrixPerspectiveFovLH(&mMatProj, D3DX_PI / 4, width / (FLOAT)height, mNear, mFar);
}

void Camera::MoveFront(float speed)
{
	float dTime = Timer::GetInstance()->GetDeltaTime();
	D3DXMatrixTranslation(&mTransform, speed*dTime, 0, 0);
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



#include "stdafx.h"
#include "Camera.h"
#include "App.h"





Camera::Camera()
: mEye(0.0f, 4.0f, -10.0f), mAt(0.0f, 1.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f)
{
}


Camera::~Camera()
{
}


void Camera::Update()
{
	RECT rc;
	GetClientRect(App::GetInstance()->GetHandleMainWindow(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3DXMatrixLookAtLH(&mMatView, &mEye, &mAt,&mUp );
	D3DXMatrixPerspectiveFovLH(&mMatProj, D3DX_PI / 4, width / (FLOAT)height, mNear, mFar);
}

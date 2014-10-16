#include "stdafx.h"
#include "Camera.h"






Camera::Camera()
: mEye(0.0f, 1.0f, -5.0f), mAt(0.0f, 1.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f)
{
}


Camera::~Camera()
{
}


void Camera::Update()
{
	D3DXMatrixLookAtLH(&mMatView, &mEye, &mAt,&mUp );
	D3DXMatrixPerspectiveFovLH(&mMatProj, D3DX_PI / 4, 1.0f, mNear, mFar );
}

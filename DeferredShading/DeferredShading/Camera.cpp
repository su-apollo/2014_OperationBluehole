#include "stdafx.h"
#include "Camera.h"
#include "App.h"








void Camera::Update()
{
	RECT rc;
	GetClientRect(App::GetInstance()->GetHandleMainWindow(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3DXMatrixLookAtLH(&mMatView, &mEye, &mAt,&mUp );
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

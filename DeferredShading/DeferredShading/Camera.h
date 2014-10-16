#pragma once





class Camera
{
public:
	Camera();
	~Camera();

	void Update();

	D3DXMATRIX GetMatView() { return mMatView; };
	D3DXMATRIX GetMatProj() { return mMatProj; };

private:

	D3DXVECTOR3 mEye;
	D3DXVECTOR3 mAt;
	D3DXVECTOR3 mUp;

	float		mNear = 1.0f;
	float		mFar = 30.0f;

	D3DXMATRIX	mMatView;
	D3DXMATRIX	mMatProj;
};


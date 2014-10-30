#pragma once



class Material
{
public:
	Material();
	~Material();



private:

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;


};




class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();





};

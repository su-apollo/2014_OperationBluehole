#pragma once

#include <fbxsdk.h>
#include <xnamath.h>

struct Vertex
{
	D3DXVECTOR3 mPos;
	D3DXVECTOR3 mNormal;
	float mTU;
	float mTV;

};

struct Indices
{
	int i0, i1, i2;
};


static const int EMAX_LIGHT = 2;

struct EVSConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

struct EPSConstantBuffer
{
	D3DXVECTOR4 vLightDir[2];
	D3DXVECTOR4 vLightColor[2];
};


class Elin
{
public:
	Elin();
	~Elin();

	BOOL Init();
	BOOL LoadFBX();
	void CleanUp();

	BOOL CompileShader();
	BOOL CreateBuffer();

	void Render();
	void Release();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;

	void ProcessGeometry(FbxNode* inNode);

	std::string GetFileName(const char* fileName);

	std::vector<Vertex> mVertices;
	//std::vector<Indices> mIndex;
	std::vector<int>mIndices;
	unsigned int mPolygonCount;

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();

	BOOL	CreateVertexBuff();
	BOOL	CreateIndexBuff();
	BOOL	CreateConstBuff();

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;
	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	D3DXMATRIX				mWorld;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;

};


#pragma once

#include <fbxsdk.h>
#include <xnamath.h>
#include "Singleton.h"
#include "ElinObj.h"

struct Vertex
{
	D3DXVECTOR3 mPos;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mUV;

};

struct Mesh
{
	std::vector<Vertex> mVertex;
	std::vector<unsigned int> mIndices;
	int mNumPolygon;
	int mNumVertex;
	int mNumIndex;

};

struct Indices
{
	int i0, i1, i2;
};



class Elin :public Singleton<Elin>
{
public:
	Elin();
	~Elin();

	BOOL Init();
	BOOL LoadFBX();
	void CleanUp();

	BOOL CompileShader();
	BOOL CreateMeshBuffer(Mesh* mesh);
	BOOL LoadTexture();


	void Render();
	void Release();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;

	void ProcessGeometry(FbxNode* inNode);

	std::string GetFileName(const char* fileName);

	std::vector<Vertex> mVertices;
	std::vector<int>mIndices;
	
	std::vector<Mesh*> mModel;

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();

	BOOL	CreateMeshVB(Mesh* mesh);
	BOOL	CreateMeshIB(Mesh* mesh);
	BOOL	CreateMeshCB(Mesh* mesh);

	void RenderMesh();


	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;
	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	ID3D11ShaderResourceView*	mTextureRV = NULL;
	ID3D11SamplerState*			mSamplerLinear = NULL;

	D3DXMATRIX				mWorld;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;

};


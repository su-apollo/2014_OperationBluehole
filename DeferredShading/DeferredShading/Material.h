#pragma once
#include "Singleton.h"

class MaterialManager;

class Material
{
	friend MaterialManager;
public:
	Material();
	~Material();


private:
	
	ID3D11ShaderResourceView*	mTRVDiffuse = NULL;
	ID3D11ShaderResourceView*	mTRVSpecular = NULL;
	ID3D11ShaderResourceView*	mTRVNormal = NULL;

	ID3D11VertexShader*			mVertexShader = NULL;
	ID3D11PixelShader*			mPixelShader = NULL;

	ID3D11InputLayout*			mVertexLayout11 = NULL;

	ID3D11Buffer*				mVSConstBuffer = NULL;
	ID3D11Buffer*				mPSConstBuffer = NULL;

};

typedef std::shared_ptr<Material> MaterialPointer;


static const LPCSTR	MAT_VS_MAIN = "main";
static const LPCSTR MAT_VS_MODEL = "vs_4_0_level_9_3";

static const LPCSTR	MAT_PS_MAIN = "main";
static const LPCSTR MAT_PS_MODEL = "ps_4_0_level_9_3";

//먼가 부족한 material
class MaterialManager : public Singleton<MaterialManager>
{
public:
	MaterialManager();
	~MaterialManager();

	BOOL			Init();
	MaterialPointer CreateMaterial(	UINT vertexConstbuff, WCHAR* vertexShader, UINT pixelConstbuff, WCHAR* pixelShader, 
									WCHAR* diffusePath, WCHAR* normalPath, WCHAR* specularPath = NULL);

private:

	BOOL			CreateTextureSR(WCHAR* diffusePath, WCHAR* specularPath, WCHAR* normalPath, MaterialPointer mat);

	BOOL			CompileVertexShader(WCHAR* shaderPath, MaterialPointer mat);
	BOOL			CompilePixelShader(WCHAR* shaderPath, MaterialPointer mat);
	BOOL			CreateConstBuffer(UINT vertexBuffSize, UINT pixelBuffSize, MaterialPointer mat);

	HRESULT			CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};

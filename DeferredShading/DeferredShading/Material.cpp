#include "stdafx.h"
#include "Material.h"
#include "Renderer.h"
#include "App.h"

Material::Material()
{
}


Material::~Material()
{
}




MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{

}

BOOL MaterialManager::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();

	return TRUE;
}

MaterialPointer MaterialManager::CreateMaterial(UINT vertexConstbuff, WCHAR* vertexShader, 
												UINT pixelConstbuff, WCHAR* pixelShader, 
												WCHAR* diffusePath, WCHAR* normalPath, WCHAR* specularPath /*= NULL*/)
{
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();
	MaterialPointer mat(new Material);

	if (!CompileVertexShader(vertexShader, mat))
	{
		MessageBox(hWnd, L"CreateMaterial VertexShader Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		exit(1);
	}
	
	if (!CompilePixelShader(pixelShader, mat))
	{
		MessageBox(hWnd, L"CreateMaterial PixelShader Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		exit(1);
	}

	if (!CreateConstBuffer(vertexConstbuff, pixelConstbuff, mat))
	{
		MessageBox(hWnd, L"CreateMaterial ConstBuffer Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		exit(1);
	}

	if (!CreateTextureSR(diffusePath, specularPath, normalPath, mat))
	{
		MessageBox(hWnd, L"CreateMaterial Texture Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		exit(1);
	}

	return mat;
}

BOOL MaterialManager::CompileVertexShader(WCHAR* shaderPath, MaterialPointer mat)
{
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(shaderPath, MAT_VS_MAIN, MAT_VS_MODEL, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mat->mVertexShader);

	if (FAILED(hr))
	{
		SafeRelease(pVSBlob);
		return FALSE;
	}

	// input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mat->mVertexLayout11);

	SafeRelease(pVSBlob);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL MaterialManager::CompilePixelShader(WCHAR* shaderPath, MaterialPointer mat)
{
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(shaderPath, MAT_PS_MAIN, MAT_PS_MODEL, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mat->mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL MaterialManager::CreateConstBuffer(UINT vertexBuffSize, UINT pixelBuffSize, MaterialPointer mat)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.ByteWidth = vertexBuffSize;
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mat->mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	bd.ByteWidth = pixelBuffSize;
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mat->mPSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

HRESULT MaterialManager::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

BOOL MaterialManager::CreateTextureSR(WCHAR* diffusePath, WCHAR* specularPath, WCHAR* normalPath, MaterialPointer mat)
{
	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, diffusePath, NULL, NULL, &mat->mTRVDiffuse, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, normalPath, NULL, NULL, &mat->mTRVNormal, NULL);
	if (FAILED(hr))
		return FALSE;

	// specular가 없을 경우 그냥 넘김
	if (!specularPath)
		return TRUE;

	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, specularPath, NULL, NULL, &mat->mTRVSpecular, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}




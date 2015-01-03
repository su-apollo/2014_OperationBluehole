#include "stdafx.h"
#include "Billboard.h"
#include "Renderer.h"
#include "App.h"
#include "RTManager.h"
#include "Camera.h"
#include "SamplerManager.h"

Billboard::Billboard() : mPos(0, 0, 0, 0)
{
}

Billboard::~Billboard()
{
	SafeRelease(mPixelShader);
	SafeRelease(mVSConstBuffer);
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
}

BOOL Billboard::Init(const LPCWSTR path)
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();

	if (!CompileShader())
	{
		MessageBox(hWnd, L"Billboard CompileShader Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!CreateConstBuffer())
	{
		MessageBox(hWnd, L"Billboard CreateConstBuffer Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!CreateQuad())
	{
		MessageBox(hWnd, L"Billboard Quad Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!LoadTexture(path))
	{
		MessageBox(hWnd, L"Billboard Load Texture Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

void Billboard::Render()
{
	// set lay out
	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	// set vertex
	UINT stride = sizeof(QuadVertex);
	UINT offset = 0;
	mD3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// set index
	mD3DDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// set primitive
	mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set shader
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mVSConstBuffer);

	// set const buff
	BillboardConstBuffer vcb;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &matView);
	matWorld._41 = mPos.x;
	matWorld._42 = mPos.y;
	matWorld._43 = mPos.z;
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();

	D3DXMatrixTranspose(&matWorld, &matWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	
	vcb.mWorld = matWorld;
	vcb.mView = matView;
	vcb.mProjection = matProj;
	
	mD3DDeviceContext->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcb, 0, 0);

	// set Texture and sampler
	mD3DDeviceContext->PSSetShaderResources(0, 1, &mTexture);

	ID3D11SamplerState* linearSampler = SamplerManager::GetInstance()->GetLinearSampler();
	mD3DDeviceContext->PSSetSamplers(0, 1, &linearSampler);

	// draw
	mD3DDeviceContext->DrawIndexed(6, 0, 0);
}

BOOL Billboard::CreateQuad()
{
	QuadVertex verts[4] =
	{
		{ D3DXVECTOR3(5, 5, 0), D3DXVECTOR2(1, 0) },
		{ D3DXVECTOR3(5, -5, 0), D3DXVECTOR2(1, 1) },
		{ D3DXVECTOR3(-5, -5, 0), D3DXVECTOR2(0, 1) },
		{ D3DXVECTOR3(-5, 5, 0), D3DXVECTOR2(0, 0) }
	};

	D3D11_BUFFER_DESC desc;
	// immutable
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(verts);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = verts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	mD3DDevice->CreateBuffer(&desc, &initData, &mVertexBuffer);
	if (FAILED(hr))
		return FALSE;

	unsigned short indices[6] = { 0, 1, 2, 2, 3, 0 };

	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(indices);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	mD3DDevice->CreateBuffer(&desc, &initData, &mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Billboard::CompileShader()
{
	// vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(mVertexShaderPath, mVertexShaderMain, mVertexShaderModel, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mVertexShader);

	if (FAILED(hr))
	{
		SafeRelease(pVSBlob);
		return FALSE;
	}

	// input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);
	if (FAILED(hr))
		return FALSE;

	// pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(mPixelShaderPath, mPixelShaderMain, mPixelShaderModel, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

HRESULT Billboard::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

BOOL Billboard::CreateConstBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(BillboardConstBuffer);
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Billboard::LoadTexture(const LPCWSTR path)
{
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, path, NULL, NULL, &mTexture, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}




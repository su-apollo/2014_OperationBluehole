#include "stdafx.h"
#include "Cube.h"
#include "Renderer.h"




Cube::Cube()
{
	D3DXMatrixIdentity(&mWorld);
}


Cube::~Cube()
{
}

BOOL Cube::FillBuffer()
{
	HRESULT hr = S_OK;

	//IA - Input Assembler Stage
	//1. 입력버퍼 생성
	//2. layout 생성(vertex 정의)
	//3. IASetinputlayout, IASetbuffer를 통해서 IA단계에 생성 정보들을 전달
	//4. IASetPrimitiveTopology를 통해 설정
	//5. dpcall

	// Create vertex buffer
	CubeVertex vertices[] =
	{
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CubeVertex)* 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &InitData, &mVertexBuffer);
	if (FAILED(hr))
		return FALSE;

	// Set vertex buffer
	UINT stride = sizeof(CubeVertex);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Create index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		0, 5, 4,
		1, 5, 0,

		3, 4, 7,
		0, 4, 3,

		1, 6, 5,
		2, 6, 1,

		2, 7, 6,
		3, 7, 2,

		6, 4, 5,
		7, 4, 6,
	};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)* 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &InitData, &mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	// Set index buffer
	Renderer::GetInstance()->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return TRUE;
}



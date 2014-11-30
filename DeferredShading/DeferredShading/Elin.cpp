#include "stdafx.h"
#include "Elin.h"
#include "Renderer.h"
#include "Timer.h"

Elin::Elin()
{
}


Elin::~Elin()
{
}

BOOL Elin::Init()
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	mDeviceContext = Renderer::GetInstance()->GetDeviceContext();

	if (!LoadFBX(device, ELIN_PATH))
		return FALSE;

	if (!CompileShader(device, ELIN_VS_PATH, ELIN_VS_MAIN, ELIN_VS_MODEL, ELIN_PS_PATH, ELIN_PS_MAIN, ELIN_PS_MODEL))
		return FALSE;

	if (!CreateVSConstBuffer(device, sizeof(VS_CONSTBUFF_DATA)))
		return FALSE;

	return TRUE;
}

BOOL Elin::MaterialConstruction(ID3D11Device* device)
{
	mMeshNodeArray[0].materialData.diffuseTexPath = ELIN_TEXTURE_HAND_DIFF;
	mMeshNodeArray[0].materialData.specularTexPath = ELIN_TEXTURE_HAND_SPEC;
	mMeshNodeArray[0].materialData.normalTexPath = ELIN_TEXTURE_HAND_NORM;

	mMeshNodeArray[1].materialData.diffuseTexPath = ELIN_TEXTURE_LEG_DIFF;
	mMeshNodeArray[1].materialData.specularTexPath = ELIN_TEXTURE_LEG_SPEC;
	mMeshNodeArray[1].materialData.normalTexPath = ELIN_TEXTURE_LEG_NORM;

	mMeshNodeArray[2].materialData.diffuseTexPath = ELIN_TEXTURE_BODY_DIFF;
	mMeshNodeArray[2].materialData.specularTexPath = ELIN_TEXTURE_BODY_SPEC;
	mMeshNodeArray[2].materialData.normalTexPath = ELIN_TEXTURE_BODY_NORM;

	mMeshNodeArray[3].materialData.diffuseTexPath = ELIN_TEXTURE_HAIR_DIFF;
	mMeshNodeArray[3].materialData.specularTexPath = ELIN_TEXTURE_HAIR_SPEC;
	mMeshNodeArray[3].materialData.normalTexPath = ELIN_TEXTURE_HAIR_NORM;

	mMeshNodeArray[4].materialData.diffuseTexPath = ELIN_TEXTURE_FACE_DIFF;
	mMeshNodeArray[4].materialData.specularTexPath = ELIN_TEXTURE_FACE_SPEC;
	mMeshNodeArray[4].materialData.normalTexPath = ELIN_TEXTURE_FACE_NORM;

	for (size_t i = 0; i < mMeshNodeArray.size(); ++i)
	{
		BOOL result = CreateMeshTexture(device, mMeshNodeArray[i]);
		if (!result)
			return FALSE;
	}

	return TRUE;
}

BOOL Elin::CreateMeshTexture(ID3D11Device* device, MESH_NODE& meshNode)
{
	hr = D3DX11CreateShaderResourceViewFromFile(device, meshNode.materialData.diffuseTexPath, NULL, NULL, &meshNode.materialData.pSRVDiffuse, NULL);
	if (FAILED(hr))
		return FALSE;
	hr = D3DX11CreateShaderResourceViewFromFile(device, meshNode.materialData.specularTexPath, NULL, NULL, &meshNode.materialData.pSRVSpecular, NULL);
	if (FAILED(hr))
		return FALSE;
	hr = D3DX11CreateShaderResourceViewFromFile(device, meshNode.materialData.normalTexPath, NULL, NULL, &meshNode.materialData.pSRVNormal, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

void Elin::RenderAll()
{
	D3DXMATRIX matRotate;
	float rotation = 0;
	if (mIsRotating)
		rotation = Timer::GetInstance()->GetDeltaTime();

	D3DXMatrixRotationY(&matRotate, rotation);
	mWorld *= matRotate;

	Mesh::RenderAll(mDeviceContext);
}

void Elin::IsRotate()
{
	if (mIsRotating)
		mIsRotating = FALSE;
	else
		mIsRotating = TRUE;
}



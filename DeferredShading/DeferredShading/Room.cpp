#include "stdafx.h"
#include "Room.h"
#include "Renderer.h"

Room::Room()
{
}


Room::~Room()
{
}

BOOL Room::Init()
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	mDeviceContext = Renderer::GetInstance()->GetDeviceContext();

	if (!LoadFBX(device, ROOM_PATH))
		return FALSE;

	if (!CompileShader(device, ROOM_VS_PATH, ROOM_VS_MAIN, ROOM_VS_MODEL, ROOM_PS_PATH, ROOM_PS_MAIN, ROOM_PS_MODEL))
		return FALSE;

	if (!CreateVSConstBuffer(device, sizeof(VS_CONSTBUFF_DATA)))
		return FALSE;

	return TRUE;
}

BOOL Room::MaterialConstruction(ID3D11Device* device)
{
	mMeshNodeArray[0].materialData.diffuseTexPath = ROOM_ITEM_TEXTURE_DIFF;
	mMeshNodeArray[0].materialData.normalTexPath = ROOM_ITEM_TEXTURE_NORM;

	mMeshNodeArray[1].materialData.diffuseTexPath = ROOM_TEXTURE_DIFF;
	mMeshNodeArray[1].materialData.normalTexPath = ROOM_TEXTURE_NORM;

	for (size_t i = 0; i < mMeshNodeArray.size(); ++i)
	{
		BOOL result = CreateMeshTexture(device, mMeshNodeArray[i]);
		if (!result)
			return FALSE;
	}

	return TRUE;
}

BOOL Room::CreateMeshTexture(ID3D11Device* device, MESH_NODE& meshNode)
{
	hr = D3DX11CreateShaderResourceViewFromFile(device, meshNode.materialData.diffuseTexPath, NULL, NULL, &meshNode.materialData.pSRVDiffuse, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = D3DX11CreateShaderResourceViewFromFile(device, meshNode.materialData.normalTexPath, NULL, NULL, &meshNode.materialData.pSRVNormal, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

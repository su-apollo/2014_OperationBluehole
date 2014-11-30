#pragma once
#include "Mesh.h"

class Room : public Mesh
{
public:
	Room();
	~Room();

	BOOL Init();

private:

	// material은 하드코딩으로 구현되어있음
	BOOL	MaterialConstruction(ID3D11Device* device);
	BOOL	CreateMeshTexture(ID3D11Device* device, MESH_NODE& meshNode);

	const CHAR*			ROOM_PATH = "bg/BG_ColorBleedTest00_SM.FBX";

	WCHAR*				ROOM_VS_PATH = L"ElinVertexShader.hlsl";
	const LPCSTR		ROOM_VS_MAIN = "main";
	const LPCSTR		ROOM_VS_MODEL = "vs_5_0";

	WCHAR*				ROOM_PS_PATH = L"ElinPixelShader.hlsl";
	const LPCSTR		ROOM_PS_MAIN = "main";
	const LPCSTR		ROOM_PS_MODEL = "ps_5_0";

	const LPCWSTR		ROOM_TEXTURE_DIFF = L"bg/BG_ColorBleedTest00_diff.bmp";
	const LPCWSTR		ROOM_TEXTURE_NORM = L"bg/BG_ColorBleedTest00_norm.bmp";

	const LPCWSTR		ROOM_ITEM_TEXTURE_DIFF = L"bg/BG_ColorBleedTest01_diff.bmp";
	const LPCWSTR		ROOM_ITEM_TEXTURE_NORM = L"bg/BG_ColorBleedTest01_norm.bmp";

	ID3D11DeviceContext*    mDeviceContext = NULL;
};


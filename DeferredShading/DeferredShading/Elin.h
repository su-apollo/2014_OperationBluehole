#pragma once
#include "Mesh.h"

class Elin : public Mesh
{
public:
	Elin();
	~Elin();

	BOOL Init();
	void RenderAll();
	void IsRotate();

private:

	// material은 하드코딩으로 구현되어있음
	BOOL	MaterialConstruction(ID3D11Device* device);
	BOOL	CreateMeshTexture(ID3D11Device* device, MESH_NODE& meshNode);

	const CHAR*		ELIN_PATH = "ElinModel/Popori_F_H00_dance_good.FBX";
	
	WCHAR*			ELIN_VS_PATH = L"ElinVertexShader.hlsl";
	const LPCSTR	ELIN_VS_MAIN = "main";
	const LPCSTR	ELIN_VS_MODEL = "vs_5_0";

	WCHAR*			ELIN_PS_PATH = L"ElinPixelShader.hlsl";
	const LPCSTR	ELIN_PS_MAIN = "main";
	const LPCSTR	ELIN_PS_MODEL = "ps_5_0";

	const LPCWSTR	ELIN_TEXTURE_FACE_DIFF = L"ElinModel/Popori_F_Face01_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_FACE_NORM = L"ElinModel/Popori_F_Face01_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_FACE_SPEC = L"ElinModel/Popori_F_Face01_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_BODY_DIFF = L"ElinModel/Popori_F_H00_Body_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_BODY_NORM = L"ElinModel/Popori_F_H00_Body_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_BODY_SPEC = L"ElinModel/Popori_F_H00_Body_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_HAND_DIFF = L"ElinModel/Popori_F_H00_Hand_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAND_NORM = L"ElinModel/Popori_F_H00_Hand_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAND_SPEC = L"ElinModel/Popori_F_H00_Hand_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_LEG_DIFF = L"ElinModel/Popori_F_H00_Leg_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_LEG_NORM = L"ElinModel/Popori_F_H00_Leg_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_LEG_SPEC = L"ElinModel/Popori_F_H00_Leg_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_HAIR_DIFF = L"ElinModel/popori_F_hair07_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAIR_NORM = L"ElinModel/popori_F_hair07_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAIR_SPEC = L"ElinModel/popori_F_hair07_spec.bmp";

	BOOL					mIsRotating = TRUE;
	ID3D11DeviceContext*    mDeviceContext = NULL;
};


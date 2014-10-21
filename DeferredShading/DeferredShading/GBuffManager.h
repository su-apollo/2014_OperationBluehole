#pragma once
#include "Singleton.h"
#include "GBuffer.h"





// todo : ȭ��ũ�Ⱑ ����� ���?
// todo : depthstendcil�� ����� �δ°� ������?
class GBuffManager : public Singleton<GBuffManager>
{
public:
	GBuffManager();
	~GBuffManager();

	BOOL Init();
	BOOL CreateGBuffers();
	void SetRenderTargetToGBuff();

	ID3D11Texture2D* GetNormalBuff() { return mNormalsBuff.GetTexture(); }
	ID3D11Texture2D* GetAlbedoBuff() { return mAlbedoBuff.GetTexture(); }

private:

	void GetWindowSize(HWND hWnd);

	// render targets
	GBuffer mNormalsBuff;
	GBuffer mAlbedoBuff;


	// get from renderer
	UINT mWinWidth = 0;
	UINT mWinHeight = 0;
	ID3D11DeviceContext* mD3DDeviceContext = NULL;
	ID3D11DepthStencilView* mDepthStencilView = NULL;
};


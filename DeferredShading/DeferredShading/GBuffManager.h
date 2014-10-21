#pragma once
#include "Singleton.h"
#include "GBuffer.h"




// todo : 화면크기가 변경될 경우?
// todo : depthstendcil은 여기다 두는게 좋을까?
class GBuffManager : public Singleton<GBuffManager>
{
public:
	GBuffManager();
	~GBuffManager();

	BOOL Init();
	BOOL CreateGBuffers();
	void SetRenderTargetToGBuff();

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


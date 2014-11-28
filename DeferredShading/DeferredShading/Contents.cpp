#include "stdafx.h"
#include "App.h"
#include "Contents.h"
#include "LightManager.h"
#include "InputDispatcher.h"
#include "Camera.h"
#include "ProcessManager.h"
#include "PostProcessor.h"
#include "Renderer.h"

Contents::Contents()
{
}

Contents::~Contents()
{
}

void Contents::Init()
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	InputDispatch(VK_ESCAPE, [](){ ProcessManager::GetInstance()->Stop(); });
	InputDispatch('Z', [](){ PostProcessor::GetInstance()->ChangeKernelRadius(0.05f); });
	InputDispatch('X', [](){ PostProcessor::GetInstance()->ChangeKernelRadius(-0.05f); });
	
	InputDispatch('W', [](){ Camera::GetInstance()->Walk(30); });
	InputDispatch('S', [](){ Camera::GetInstance()->Walk(-30); });
	InputDispatch('D', [](){ Camera::GetInstance()->Strafe(30); });
	InputDispatch('A', [](){ Camera::GetInstance()->Strafe(-30); });
	InputDispatch('P', [](){ if (!InputDispatcher::GetInstance()->IsPressed('P')) Renderer::GetInstance()->ElinRotate(); });

	//Cameramove
	MouseDispatch(MouseStatusType::MOUSE_LDOWN,
		[&](int x, int y){
		mCurrentMouseX = x;
		mCurrentMouseY = y;
	});

	MouseDispatch(MouseStatusType::MOUSE_LPRESSED,
		[&](int x, int y){
		
		//HWND hwnd = App::GetInstance()->GetHandleMainWindow();

		int dx = x - mCurrentMouseX;
		int dy = y - mCurrentMouseY;
		Camera::GetInstance()->Rotate(D3DXVECTOR3(dx*0.01f, dy*0.01f, 0.0f));
	});

	LightManager::GetInstance()->CreatePointLights(MAX_LIGHT);

	//todo : ºû À§Ä¡ ¼³Á¤
	LightManager::GetInstance()->mPLightList[0]->mPos = D3DXVECTOR4(15.f, 60.0f, -50.0f, 1.0f);
	LightManager::GetInstance()->mPLightList[0]->mColor = D3DXVECTOR4(1, 1, 1, 1);

	LightManager::GetInstance()->mPLightList[1]->mPos = D3DXVECTOR4(-15.f, 60.0f, 50.f, 1.0f);
	LightManager::GetInstance()->mPLightList[1]->mColor = D3DXVECTOR4(1, 1, 1, 1);

}

void Contents::Render()
{

}



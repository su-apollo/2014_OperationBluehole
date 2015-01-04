#include "stdafx.h"
#include "App.h"
#include "Contents.h"
#include "LightManager.h"
#include "InputDispatcher.h"
#include "Camera.h"
#include "ProcessManager.h"
#include "PostProcessor.h"
#include "Renderer.h"
#include "Timer.h"

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
	InputDispatch('P', 
		[](){ 
		if (!InputDispatcher::GetInstance()->IsPressed('P')) 
			Renderer::GetInstance()->ElinRotate(); 
	});

	InputDispatch('O', 
		[](){ 
		if (!InputDispatcher::GetInstance()->IsPressed('O')) 
			PostProcessor::GetInstance()->mBillSwitch = !PostProcessor::GetInstance()->mBillSwitch; 
	});

	InputDispatch('I', 
		[&](){ 
		float delta = Timer::GetInstance()->GetDeltaTime();
		mLightRadius -= delta * 10;
	});
	InputDispatch('U',
		[&](){
		float delta = Timer::GetInstance()->GetDeltaTime();
		mLightRadius += delta * 10;
	});

	//Cameramove
	MouseDispatch(MouseStatusType::MOUSE_LDOWN,
		[&](int x, int y){
		mCurrentMouseX = x;
		mCurrentMouseY = y;
	});

	MouseDispatch(MouseStatusType::MOUSE_LPRESSED,
		[&](int x, int y){
		
		//HWND hwnd = App::GetInstance()->GetHandleMainWindow();

		dx = x - mCurrentMouseX;
		dy = y - mCurrentMouseY;
		Camera::GetInstance()->Rotate(D3DXVECTOR3(mCameraX + dx*0.01f, mCameraY + dy*0.01f, 0.0f));
	});

	MouseDispatch(MouseStatusType::MOUSE_LUP,
		[&](int x, int y){ 
		mCameraX += dx*0.01f;
		mCameraY += dy*0.01f;
	});

	LightManager::GetInstance()->CreatePointLights(MAX_LIGHT);

	//todo : �� ��ġ ����
	LightManager::GetInstance()->mPLightList[0]->mPos = D3DXVECTOR4(15.f, 60.0f, -50.0f, 1.0f);
	LightManager::GetInstance()->mPLightList[0]->mColor = D3DXVECTOR4(1, 1, 1, 0);

	LightManager::GetInstance()->mPLightList[1]->mPos = D3DXVECTOR4(-15.f, 60.0f, 50.f, 1.0f);
	LightManager::GetInstance()->mPLightList[1]->mColor = D3DXVECTOR4(0, 0, 0, 0);

}

void Contents::Update()
{
	mCurrentTime += Timer::GetInstance()->GetDeltaTime();
	if (mCurrentTime / 360)
		mCurrentTime -= 360;
	float r = mCurrentTime * (3.14f / 180);
	float x = cos(r) * mLightRadius;
	float z = sin(r) * mLightRadius;
	LightManager::GetInstance()->mPLightList[0]->mPos = D3DXVECTOR4(x, 70.f, z, 1.0f);
}



#include "stdafx.h"
#include "Contents.h"
#include "LightManager.h"
#include "InputDispatcher.h"
#include "Camera.h"
#include "ProcessManager.h"

Contents::Contents()
{
}

Contents::~Contents()
{
}

void Contents::Init()
{
	InputDispatch(VK_ESCAPE, [](){ ProcessManager::GetInstance()->Stop(); });

	LightManager::GetInstance()->CreatePointLights(MAX_LIGHT);

	//todo : ºû À§Ä¡ ¼³Á¤
	LightManager::GetInstance()->mPLightList[1]->mPos = D3DXVECTOR4(-50.f, 0.f, -50.f, 1.0f);
	LightManager::GetInstance()->mPLightList[1]->mColor = D3DXVECTOR4(1, 1, 1, 1);

}

void Contents::Render()
{

}



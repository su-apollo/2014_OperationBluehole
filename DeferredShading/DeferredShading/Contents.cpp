#include "stdafx.h"
#include "Contents.h"
#include "Config.h"
#include "LightManager.h"



Contents::Contents()
{
}


Contents::~Contents()
{
}

void Contents::Init()
{
	LightManager::GetInstance()->CreatePointLights(MAX_LIGHT);

	//todo : �� ��ġ ����
	LightManager::GetInstance()->mPLightList[1]->mPos = D3DXVECTOR4(-50.f, 0.f, -50.f, 1.0f);
	LightManager::GetInstance()->mPLightList[1]->mColor = D3DXVECTOR4(1, 1, 1, 1);
}

void Contents::Render()
{

}


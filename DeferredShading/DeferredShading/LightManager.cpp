#include "stdafx.h"
#include "LightManager.h"
#include "DirectionalLight.h"


LightManager::LightManager()
{
	mLightBill.Init(mBillBoardTexturePath);
}


LightManager::~LightManager()
{
}

void LightManager::CreateDirectionalLights(int num)
{
	// todo : more faster

	for (int i = 0; i < num; ++i)
		mDLightList.push_back(DLightPointer(new DirectionalLight));
}

void LightManager::CreatePointLights(int num)
{
	for (int i = 0; i < num; ++i)
		mPLightList.push_back(PLightPointer(new PointLight));
}

void LightManager::RenderBill()
{
	// 더욱 최적화 할 수 있을 것 같음
	UINT num = mPLightList.size();
	D3DXVECTOR4* posArray = new D3DXVECTOR4[num];

	int i = 0;
	for (auto iter : mPLightList)
	{
		posArray[i] = iter->mPos;
		++i;
	}

	mLightBill.RenderInstanced(num, posArray);
	delete[] posArray;
}


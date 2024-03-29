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
	UINT num = mPLightList.size();
	D3DXVECTOR4* posArray = new D3DXVECTOR4[num];

	int i = 0;
	for (auto& iter : mPLightList)
	{
		if (iter->mColor != D3DXVECTOR4(0, 0, 0, 0))
		{
			posArray[i] = iter->mPos;
			++i;
		}
	}

	mLightBill.RenderInstanced(num, posArray);
	delete[] posArray;
}

void LightManager::PointLightTurnOff()
{
	for (auto& iter : mPLightList)
		iter->mColor = D3DXVECTOR4(0, 0, 0, 0);
}


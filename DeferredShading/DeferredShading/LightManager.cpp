#include "stdafx.h"
#include "LightManager.h"
#include "DirectionalLight.h"







LightManager::LightManager()
{
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


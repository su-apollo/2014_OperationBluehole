#include "stdafx.h"
#include "LightManager.h"
#include "DirectionalLight.h"







LightManager::LightManager()
{
}


LightManager::~LightManager()
{
}

void LightManager::CreateDirectionalLight(int num)
{
	// todo : more faster

	for (int i = 0; i < num; ++i)
		mDLightList.push_back(DLightPointer(new DirectionalLight));
}


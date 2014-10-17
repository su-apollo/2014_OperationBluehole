#pragma once
#include "Singleton.h"
#include "DirectionalLight.h"




typedef std::shared_ptr<DirectionalLight> DLightPointer;
typedef std::vector<DLightPointer> DLightList;



class LightManager : public Singleton<LightManager>
{

public:
	LightManager();
	~LightManager();

	void CreateDirectionalLight(int num);
		
	DLightList	mDLightList;
	

};


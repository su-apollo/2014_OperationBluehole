#pragma once
#include "Singleton.h"




class DirectionalLight;
typedef std::shared_ptr<DirectionalLight> DLightPointer;
typedef std::vector<DLightPointer> DLightList;



class LightManager : public Singleton<LightManager>
{

public:
	LightManager();
	~LightManager();







private:
	
	DLightList	mDLightList;
	UINT		mDLightNum;
	

};


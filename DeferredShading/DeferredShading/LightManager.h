#pragma once
#include "Singleton.h"
#include "DirectionalLight.h"
#include "PointLight.h"



typedef std::shared_ptr<DirectionalLight> DLightPointer;
typedef std::vector<DLightPointer> DLightList;

typedef std::shared_ptr<PointLight> PLightPointer;
typedef std::vector<PLightPointer> PLightList;



class LightManager : public Singleton<LightManager>
{

public:
	LightManager();
	~LightManager();

	void RenderBill();

	void CreateDirectionalLights(int num);
	void CreatePointLights(int num);

	DLightList mDLightList;
	PLightList mPLightList;

	Billboard	mLightBill;

	const LPCWSTR mBillBoardTexturePath = L"light_bulb.png";
};


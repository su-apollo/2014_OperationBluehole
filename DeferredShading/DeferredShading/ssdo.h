#pragma once
#include "Singleton.h"

enum SHADERMODE{
	SM_NONE =1,
	SM_AO,
	SM_AO_BLUR,
	SM_BOUNCE,
	SM_BOUNCE_BLUR,
	SM_SSAO,
	SM_SSDO_BOUNCE,
	SM_SSDO
};



class SSDO : public Singleton < SSDO >
{
public:
	SSDO();
	~SSDO();

	void ChangeShaderMode(SHADERMODE shaderMode) { mCurrentMode = shaderMode; }
private:
	SHADERMODE mCurrentMode = SM_SSDO;
};



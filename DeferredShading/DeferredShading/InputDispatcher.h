#pragma once
#include "Singleton.h"

enum KeyStatusType
{
	KEY_NONE = 0,
	KEY_DOWN,
	KEY_PRESSED,
	KEY_UP,
	KEY_STATUS_NUM
};

struct KeyInput
{
	unsigned char mKeyValue = 0;
	KeyStatusType mKeyState = KEY_NONE;
};

struct MousePosInfo
{
	MousePosInfo(int x, int y)
	{
		mPosX = x;
		mPosY = y;
	}

	int mPosX;
	int mPosY;
};

class InputDispatcher : public Singleton<InputDispatcher>
{
public:
	InputDispatcher();
	~InputDispatcher();

	//void EventKeyInput(KeyInput key);




private:
	//std::list<KeyInput>			mKeyInputList;
	//std::array<bool, MAX_KEY>	mIsKeyPressed;

	


};


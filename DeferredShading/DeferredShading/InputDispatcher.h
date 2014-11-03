#pragma once
#include "Singleton.h"
#include "KeyCode.h"

enum KeyStatusType
{
	KEY_NONE = 0,
	KEY_DOWN,
	KEY_PRESSED,
	KEY_UP,
	KEY_STATUS_MAX
};

struct KeyInput
{
	unsigned char mKeyValue = 0;
	KeyStatusType mKeyState = KEY_NONE;
};

struct MousePosInfo
{
	MousePosInfo(int x, int y) : mPosX(x), mPosY(y) {}
	int mPosX;
	int mPosY;
};

class InputDispatcher : public Singleton<InputDispatcher>
{
public:
	InputDispatcher();
	~InputDispatcher();

	void			EventKeyInput(KeyInput key);
	void			DispatchKeyInput();

	bool			IsPressed(KeyInput key) const { return IsPressed(key.mKeyValue); }
	bool			IsPressed(unsigned char key) const { return mIsKeyPressed[key]; }

	MousePosInfo	GetMousePosition();

private:
	std::list<KeyInput>			mKeyInputList;
	std::array<bool, MAX_KEY>	mIsKeyPressed;

	MousePosInfo				mUpdatedMousePos;
};


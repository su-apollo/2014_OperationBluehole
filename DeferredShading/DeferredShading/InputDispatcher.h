#pragma once
#include "Singleton.h"
#include "KeyCode.h"
#include <functional>

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

// todo : dispatch로 넘기는 함수 인자를 받을 수 있도록 설정 
// 인자가 필요하면 말해주세요
class InputDispatcher : public Singleton<InputDispatcher>
{
	typedef std::function<void()> KeyTask;

public:
	InputDispatcher();
	~InputDispatcher();

	void			EventKeyInput(KeyInput key);
	void			RegisterKeyTask(unsigned char keyType, const KeyTask& task) { mKeyTaskTable[keyType] = task; }

	void			DispatchKeyInput();

	bool			IsPressed(KeyInput key) const { return IsPressed(key.mKeyValue); }
	bool			IsPressed(unsigned char key) const { return mIsKeyPressed[key]; }

	MousePosInfo	GetMousePosition();

private:
	std::list<KeyInput>				mKeyInputList;
	std::array<bool, MAX_KEY>		mIsKeyPressed;
	std::array<KeyTask, MAX_KEY>	mKeyTaskTable;

	MousePosInfo					mUpdatedMousePos;
};

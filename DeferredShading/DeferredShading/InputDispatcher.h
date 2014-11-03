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

// 해당 키 이벤트가 발생하면 등록해둔 함수 실행
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

//함수포인터로 사용
typedef void(*KeyEventHandler)(KeyInput inputKey);
static KeyEventHandler KeyHandlerTable[MAX_KEY];

struct RegisterKeyHandler
{
	RegisterKeyHandler(unsigned char keyType, KeyEventHandler keyHandler)
	{
		KeyHandlerTable[keyType] = keyHandler;
	}
};

//선언 -> 등록 -> 정의
#define REGISTER_KEY_HANDLER( KEY_TYPE ) \
	static void Handler_##KEY_TYPE( KeyInput inputKey ); \
	static RegisterKeyHandler _register_##KEY_TYPE(KEY_TYPE, Handler_##KEY_TYPE); \
	static void Handler_##KEY_TYPE( KeyInput inputKey )

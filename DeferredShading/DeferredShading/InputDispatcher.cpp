#include "stdafx.h"
#include "InputDispatcher.h"
#include "App.h"
#include "ProcessManager.h"

//함수포인터로 사용
typedef void(*KeyEventHandler)(KeyInput inputKey);
static KeyEventHandler KeyHandlerTable[MAX_KEY];

static void DefaultKeyHandler(KeyInput inputKey)
{
}

//사용하지 않는 구조체를 하나 만듦으로서 초기화 진행
struct InitKeyHandlers
{
	InitKeyHandlers()
	{
		for (int i = 0; i < MAX_KEY; ++i)
		{
			KeyHandlerTable[i] = DefaultKeyHandler;
		}
	}
} _init_key_handlers_;

struct RegisterKeyHandler
{
	RegisterKeyHandler(unsigned char keyType, KeyEventHandler keyHandler)
	{
		KeyHandlerTable[keyType] = keyHandler;
	}
};

InputDispatcher::InputDispatcher() : mUpdatedMousePos(0, 0)
{
	mIsKeyPressed.fill(false);
}

InputDispatcher::~InputDispatcher()
{
}

void InputDispatcher::EventKeyInput(KeyInput key)
{
	switch (key.mKeyState)
	{
	case KeyStatusType::KEY_DOWN:
		mKeyInputList.push_back(key);
		mIsKeyPressed[key.mKeyValue] = true;
		break;

	case KeyStatusType::KEY_PRESSED:
		break;
	case KeyStatusType::KEY_UP:
		mIsKeyPressed[key.mKeyValue] = false;
		break;

	default:
		assert(false);
		break;
	}
}

MousePosInfo InputDispatcher::GetMousePosition()
{
	POINT point;

	GetCursorPos(&point);
	ScreenToClient(App::GetInstance()->GetHandleMainWindow(), &point);

	mUpdatedMousePos.mPosX = point.x;
	mUpdatedMousePos.mPosY = point.y;

	return mUpdatedMousePos;
}

void InputDispatcher::DispatchKeyInput()
{
	std::list<KeyInput>::iterator iter = mKeyInputList.begin();

	while (iter != mKeyInputList.end())
	{
		if (IsPressed(*(iter)))
		{
			//이게 실행 구문
			KeyHandlerTable[iter->mKeyValue](*(iter));
			iter->mKeyState = KeyStatusType::KEY_PRESSED;
			++iter;
		}
		else
		{
			(iter)->mKeyState = KeyStatusType::KEY_UP;
			//여기서 키 실행
			KeyHandlerTable[iter->mKeyValue](*(iter));

			mKeyInputList.erase(iter);
			break;
		}
	}
}

#define REGISTER_KEY_HANDLER( KEY_TYPE ) \
	static void Handler_##KEY_TYPE( KeyInput inputKey ); \
	static RegisterKeyHandler _register_##KEY_TYPE(KEY_TYPE, Handler_##KEY_TYPE); \
	static void Handler_##KEY_TYPE( KeyInput inputKey )

REGISTER_KEY_HANDLER(VK_ESCAPE)
{
	ProcessManager::GetInstance()->Stop();
}

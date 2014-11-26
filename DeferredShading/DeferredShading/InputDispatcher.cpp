#include "stdafx.h"
#include "InputDispatcher.h"
#include "App.h"
#include "ProcessManager.h"

InputDispatcher::InputDispatcher()
{
	mIsKeyPressed.fill(false);
	for (int i = 0; i < MAX_KEY; ++i)
		mKeyTaskTable[i] = [](){};

	for (int i = 0; i < MouseStatusType::MOUSE_STATUS_MAX; ++i)
		mMouseTaskTable[i] = [](int x, int y){};
}

InputDispatcher::~InputDispatcher()
{
}

void InputDispatcher::EventKeyInput(KeyInput& key)
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

void InputDispatcher::DispatchKeyInput()
{
	std::list<KeyInput>::iterator iter = mKeyInputList.begin();

	while (iter != mKeyInputList.end())
	{
		if (IsPressed(*(iter)))
		{
			//이게 실행 구문
			mKeyTaskTable[iter->mKeyValue]();
			iter->mKeyState = KeyStatusType::KEY_PRESSED;
			++iter;
		}
		else
		{
			(iter)->mKeyState = KeyStatusType::KEY_UP;
			mKeyTaskTable[iter->mKeyValue]();
			mKeyInputList.erase(iter);
			break;
		}
	}
}

void InputDispatcher::DispatchMouseInput()
{
	for (int i = 0; i < MouseStatusType::MOUSE_STATUS_MAX; ++i)
		mMouseTaskTable[i](mMouseInput.mPosX, mMouseInput.mPosY);
}




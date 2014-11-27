#include "stdafx.h"
#include "InputDispatcher.h"
#include "App.h"
#include "ProcessManager.h"

InputDispatcher::InputDispatcher()
{
	mIsKeyPressed.fill(false);
	for (int i = 0; i < MAX_KEY; ++i)
		mKeyTaskTable[i] = [](){};

	mIsMousePressed.fill(false);
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

void InputDispatcher::EventMouseInput(MouseInput& mouse)
{
	switch (mouse.mMousestate)
	{
	case MouseStatusType::MOUSE_MOVE:
		mIsMouseMove = true;
		mLastMousePosX = mouse.mPosX;
		mLastMousePosY = mouse.mPosY;
		break;
	case MouseStatusType::MOUSE_LDOWN:
		mMouseInputList.push_back(mouse);
		mIsMousePressed[MOUSE_L] = true;
		break;
	case MouseStatusType::MOUSE_LPRESSED:
		break;
	case MouseStatusType::MOUSE_LUP:
		mIsMousePressed[MOUSE_L] = false;
		break;
	case MouseStatusType::MOUSE_RDOWN:
		mMouseInputList.push_back(mouse);
		mIsMousePressed[MOUSE_R] = true;
		break;
	case MouseStatusType::MOUSE_RPRESSED:
		break;
	case MouseStatusType::MOUSE_RUP:
		mIsMousePressed[MOUSE_R] = false;
		break;
	}
}

void InputDispatcher::DispatchMouseInput()
{
	// mouse move task
	if (mIsMouseMove)
	{
		mMouseTaskTable[MOUSE_MOVE](mLastMousePosX, mLastMousePosY);
		mIsMouseMove = false;
	}
		
	// mouse input task
	std::list<MouseInput>::iterator iter = mMouseInputList.begin();
	while (iter != mMouseInputList.end())
	{
		MouseInputType inputType = MOUSE_R;
		if ((iter->mMousestate == MOUSE_LDOWN) ||
			(iter->mMousestate == MOUSE_LPRESSED))
			inputType = MOUSE_L;

		if (IsPressed(inputType))
		{
			MouseStatusType temp = iter->mMousestate;
			mMouseTaskTable[iter->mMousestate](mLastMousePosX, mLastMousePosY);

			if (iter->mMousestate == MOUSE_LDOWN)
				iter->mMousestate = MOUSE_LPRESSED;
			else if (iter->mMousestate == MOUSE_RDOWN)
				iter->mMousestate = MOUSE_RPRESSED;

			++iter;
		}
		else
		{
			mMouseTaskTable[iter->mMousestate](mLastMousePosX, mLastMousePosY);
			mMouseInputList.erase(iter);
			break;
		}
	}

}





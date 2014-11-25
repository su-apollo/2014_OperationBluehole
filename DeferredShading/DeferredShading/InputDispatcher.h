#pragma once
#include "Singleton.h"
#include <functional>

const int MAX_KEY = 256;

enum KeyStatusType
{
	KEY_NONE = 0,
	KEY_DOWN,
	KEY_PRESSED,
	KEY_UP,
	KEY_STATUS_MAX
};

enum MouseStatusType
{
	MOUSE_NONE = 0,
	MOUSE_MOVE,
	MOUSE_LDOWN,
	MOUSE_LUP,
	MOUSE_RDOWN,
	MOUSE_RUP,
	MOUSE_STATUS_MAX
};

struct KeyInput
{
	unsigned char mKeyValue = 0;
	KeyStatusType mKeyState = KEY_NONE;
};

struct MouseInput
{
	int mPosX = 0, mPosY = 0;
	MouseStatusType mMousestate = MOUSE_NONE;
};

class InputDispatcher : public Singleton<InputDispatcher>
{
	typedef std::function<void()> KeyTask;
	typedef std::function<void(int, int)> MouseTask;
	
public:
	InputDispatcher();
	~InputDispatcher();

	void			EventKeyInput(KeyInput& key);
	void			RegisterKeyTask(unsigned char keyType, const KeyTask& task) { mKeyTaskTable[keyType] = task; }
	void			DispatchKeyInput();

	bool			IsPressed(KeyInput& key) const { return IsPressed(key.mKeyValue); }
	bool			IsPressed(unsigned char key) const { return mIsKeyPressed[key]; }

	void			EventMouseInput(MouseInput& mouse) { mMouseInput = mouse; }
	void			RegisterMouseTask(MouseStatusType mouseType, const MouseTask& task) { mMouseTaskTable[mouseType] = task; }
	void			DispatchMouseInput();

private:
	std::list<KeyInput>				mKeyInputList;
	std::array<bool, MAX_KEY>		mIsKeyPressed;
	std::array<KeyTask, MAX_KEY>	mKeyTaskTable;

	std::array<MouseTask, MOUSE_STATUS_MAX> mMouseTaskTable;
	MouseInput								mMouseInput;
};

template <class F, class... Args>
void InputDispatch(unsigned char keyType, F memfunc, Args&&... args)
{
	auto task = std::bind(memfunc, std::forward<Args>(args)...);
	InputDispatcher::GetInstance()->RegisterKeyTask(keyType, task);
}

template <class F, class... Args>
void MouseDispatch(MouseStatusType mouseType, F memfunc, Args&&... args)
{
	auto task = std::bind(memfunc, std::forward<Args>(args)...);
	InputDispatcher::GetInstance()->RegisterMouseTask(mouseType, task);
}
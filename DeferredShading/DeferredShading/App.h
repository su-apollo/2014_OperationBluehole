#pragma once
#include "Singleton.h"

static const WCHAR* WINDOW_NAME = L"DefferedShading";
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class App : public Singleton<App>
{
public:
	App();
	~App();

	BOOL	CreateMainWindow(	PCWSTR lpWindowName, 
							DWORD dwStyle, 
							DWORD dwExStyle = 0,
							int x = CW_USEDEFAULT, 
							int y = CW_USEDEFAULT,
							int nWidth = WINDOW_WIDTH, 
							int nHeight = WINDOW_HEIGHT,
							HWND hWndParent = 0, 
							HMENU hMenu = 0);

	//main roof
	int		Run()const;
	BOOL	Display(int nCmdShow) const { return ShowWindow(mHandleMainWindow, nCmdShow); }

	HWND	GetHandleMainWindow() { return mHandleMainWindow; };

	UINT	GetWindowWidth() { return mWinWidth; }
	UINT	GetWindowHeight() { return mWinHeight; }

private:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT					HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)const;

	HWND		mHandleMainWindow = nullptr;
	HACCEL		mHandleAccelTable = nullptr;

	UINT		mWinWidth = 0;
	UINT		mWinHeight = 0;
};


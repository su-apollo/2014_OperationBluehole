#include "stdafx.h"
#include "App.h"
#include "Logger.h"
#include "ProcessManager.h"

App::App()
{
}


App::~App()
{
}

BOOL App::CreateMainWindow(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle /*= 0*/, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/, int nWidth /*= WINDOW_WIDTH*/, int nHeight /*= WINDOW_HEIGHT*/, HWND hWndParent /*= 0*/, HMENU hMenu /*= 0*/)
{
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WINDOW_NAME;

	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	mHandleMainWindow = CreateWindowEx(	dwExStyle, WINDOW_NAME, lpWindowName, dwStyle,
										x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

	//로깅용 콘솔 생성
#ifdef _PRINT_CONSOLE
	Logger::GetInstance()->CreateConsole();
	Logger::GetInstance()->SetLogStatus(LOG_CONSOLE);
	Log("test입니다 \n");
#endif
	return mHandleMainWindow ? TRUE : FALSE;
}

LRESULT CALLBACK App::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	App* pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (App*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->mHandleMainWindow = hwnd;
	}
	else
	{
		pThis = (App*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	if (pThis)
	{
		return pThis->HandleMessage(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}


}


int App::Run() const
{
	MSG msg = { 0, };
	
	if (!ProcessManager::GetInstance()->Init())
	{
		ProcessManager::GetInstance()->Destroy();
		ProcessManager::Release();

		MessageBox(mHandleMainWindow, L"Process Manager Initialization Error", L"Process Manager Init Error!", MB_ICONINFORMATION | MB_OK);

		return false;
	}

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!ProcessManager::GetInstance()->Process())
			PostQuitMessage(0);
	}

#ifdef _PRINT_CONSOLE
	Logger::GetInstance()->DestroyConsole();
	Logger::Release();
#endif

	ProcessManager::GetInstance()->Destroy();
	ProcessManager::Release();

	return (int)msg.wParam;
}

LRESULT App::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) const
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		//blank
	}
		return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
		return 0;

	case WM_SIZE:
	{
		RECT rect;
		GetWindowRect(mHandleMainWindow, &rect);

		LONG width = static_cast<LONG>(LOWORD(lParam));
		LONG height = static_cast<LONG>(HIWORD(lParam));

		InvalidateRect(mHandleMainWindow, NULL, FALSE);
	}
		return 0;

	case WM_SETFOCUS:
	{
		BringWindowToTop(mHandleMainWindow);
	}
		return 0;

	case WM_KILLFOCUS:
	{
		ShowCursor(true);
	}
		return 0;

	case WM_MOUSEMOVE:
	{
		int MouseX = LOWORD(lParam);
		int MouseY = HIWORD(lParam);
	}
		return 0;

	// todo : input

	default:
		return DefWindowProc(mHandleMainWindow, uMsg, wParam, lParam);
	}

	return 0;
}



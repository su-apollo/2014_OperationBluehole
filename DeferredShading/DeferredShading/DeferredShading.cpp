// DeferredShading.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DeferredShading.h"
#include "App.h"

int APIENTRY _tWinMain(	_In_ HINSTANCE hInstance,
						_In_opt_ HINSTANCE hPrevInstance,
						_In_ LPTSTR    lpCmdLine,
						_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 메모리 릭을 체크하려면 아래의 #### 칸에 릭 난 곳 { 숫자 } 표기 된 숫자를 넣어주면 됩니다.
	// _CrtSetBreakAlloc( #### );
	// _CrtSetBreakAlloc( 139 );

	if (false == App::GetInstance()->CreateMainWindow(L"DeferredShading", WS_POPUPWINDOW))
	{
		return 0;
	}

	App::GetInstance()->Display(nCmdShow);

	// roof
	int result = App::GetInstance()->Run();

	App::Release();

	return result;
}
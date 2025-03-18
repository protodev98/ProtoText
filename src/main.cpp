#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include <Windows.h>
#include "MainWindow.h"
#include "WindowCircle.h"

static int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	
	ProtoText::MainWindow mainWindow;
	ProtoText::WindowCreateSettings settings(hInstance, L"Proto Text", WS_OVERLAPPEDWINDOW);
	if(!mainWindow.Create(settings))
	{
		return 0;
	}

	//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
	ShowWindow(mainWindow.WindowHandle(), nCmdShow);

	//Run the message loop
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
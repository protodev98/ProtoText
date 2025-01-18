#ifndef _PROTO_MAIN_WINDOW_
#define _PROTO_MAIN_WINDOW_

#include <windows.h>
#include <d2d1.h>
//#pragma comment(lib, "d2d1") - Added in CMake, no need here

#include "Window.h"
#include "CommCtrl.h"
//#pragma comment (lib, "comctl32") - Added in CMake, no need here

#include "TextEditor.h"


#define IDM_NEW 1001
#define IDM_OPEN 1002
#define IDM_SAVE 1003

namespace ProtoText
{
	class MainWindow : public ProtoText::Window<MainWindow>
	{
	private:
		HWND CreateToolbar();
		HWND CreateTextEditor(HWND hwndToolbar);
		TextEditor m_TextEditor;
		HWND m_hwndToolbar;
		HWND m_hwndTextEditor;
	public:
		PCWSTR  ClassName() const;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		MainWindow();
	};
}

#endif

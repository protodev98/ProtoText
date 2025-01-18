#include "MainWindow.h"

ProtoText::MainWindow::MainWindow() :m_TextEditor(), m_hwndTextEditor(nullptr),
m_hwndToolbar(nullptr)
{

}

HWND ProtoText::MainWindow::CreateToolbar()
{
	HWND parentWindow = MainWindow::WindowHandle();
	HINSTANCE hInstance = MainWindow::ApplicationInstance();
	const int ImageListID = 0;
	const int numButtons = 3;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	//Create the toolbar
	HWND hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_WRAPABLE, 
							0, 0, 0, 0, parentWindow, NULL, hInstance, NULL);

	//Create image list
	HIMAGELIST hImageList = ImageList_Create(bitmapSize, bitmapSize, //Dimensions of individual bitmaps
							ILC_COLOR16 | ILC_MASK, //Ensures transparent background
							numButtons, 0); 

	//Set the image list
	SendMessage(hwndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)hImageList);

	//Load the button images
	SendMessage(hwndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);

	//Initialize button info
	TBBUTTON tbButtons[numButtons] =
	{
		{MAKELONG(STD_FILENEW, ImageListID), IDM_NEW, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New"},
		{MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
		{MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, 0, buttonStyles, {0}, 0, (INT_PTR)L"Save"}
	};

	//Add Buttons
	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	//Resize toolbar then show it
	SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hwndToolbar, TRUE);

	return hwndToolbar;
}

HWND ProtoText::MainWindow::CreateTextEditor(HWND hwndToolbar)
{
	HWND parentWindow = MainWindow::WindowHandle();
	HINSTANCE hInstance = MainWindow::ApplicationInstance();

	WindowCreateSettings settings(hInstance);
	settings.hwndParent = parentWindow;
	//settings.dwStyle = WS_CHILDWINDOW | WS_VISIBLE;
	settings.dwStyle = WS_CHILD | WS_VISIBLE;

	RECT parentRect;
	GetClientRect(parentWindow, &parentRect);

	RECT toolbarRect;
	GetClientRect(hwndToolbar, &toolbarRect);

	settings.x = toolbarRect.left;
	settings.y = toolbarRect.bottom;
	settings.nWidth = parentRect.right;
	settings.nHeight = parentRect.bottom;

	m_TextEditor.Create(settings);
	
	HWND hwndTextEditor = m_TextEditor.WindowHandle();

	ShowWindow(hwndTextEditor, 1);
	UpdateWindow(hwndTextEditor);

	return hwndTextEditor;
}

LPCWSTR ProtoText::MainWindow::ClassName() const
{
	return L"MainWindow";
}

LRESULT ProtoText::MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = MainWindow::WindowHandle();
	switch (uMsg)
	{

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(hwnd);
			}
			// Else: User canceled. Do nothing.
			return 0;
		}

		case WM_CREATE:
		{
			m_hwndToolbar = MainWindow::CreateToolbar();
			m_hwndTextEditor = MainWindow::CreateTextEditor(m_hwndToolbar);
			return 0;
		}

	
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			//All paint occurs here, between BeginPaint and EndPaint
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 9));
			EndPaint(hwnd, &ps);
			return 0;
		}
	

		//When MainWindow is resized
		case WM_SIZE:
		{
			//Get MainWindow resized coordinates
			RECT mainWindowRect;
			GetClientRect(hwnd, &mainWindowRect);

			RECT toolbarRect;
			GetClientRect(m_hwndToolbar, &toolbarRect);
			int toolbarHeight = toolbarRect.bottom - toolbarRect.top;
			
			MoveWindow(m_hwndToolbar, 0, 0, mainWindowRect.right, toolbarHeight, TRUE);

			MoveWindow(m_hwndTextEditor, toolbarRect.left, toolbarRect.bottom, mainWindowRect.right, mainWindowRect.bottom, TRUE);
			return 0;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
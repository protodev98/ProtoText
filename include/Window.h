#ifndef _PROTO_WINDOW_
#define _PROTO_WINDOW_

#include <Windows.h>

namespace ProtoText
{
	template <class T> void SafeRelease(T** ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

	struct WindowCreateSettings
	{
	private:
		WindowCreateSettings() = delete;
	public:
		PCWSTR lpWindowName;
		DWORD dwStyle;
		DWORD dwExStyle;
		int x;
		int y;
		int nWidth;
		int nHeight;
		HWND hwndParent;
		HMENU hMenu;
		HINSTANCE hInstance;


		explicit WindowCreateSettings(HINSTANCE hI) : lpWindowName(L"Default Window Name"), dwStyle(WS_OVERLAPPEDWINDOW), dwExStyle(0), x(CW_USEDEFAULT), 
			y(CW_USEDEFAULT), nWidth(CW_USEDEFAULT), nHeight(CW_USEDEFAULT), hwndParent(0), hMenu(0), hInstance(hI)
		{}

		explicit WindowCreateSettings(HINSTANCE hI, PCWSTR windowName, DWORD dwStyleInput) : lpWindowName(windowName), dwStyle(dwStyleInput), dwExStyle(0),
			x(CW_USEDEFAULT), y(CW_USEDEFAULT), nWidth(CW_USEDEFAULT), nHeight(CW_USEDEFAULT),hwndParent(0), hMenu(0), hInstance(hI)
		{}
	};

	template <class DERIVED_TYPE>
	class Window
	{
	protected:
		HWND m_hwnd;
		HINSTANCE m_hInstance;

	public:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			DERIVED_TYPE* pThis = nullptr;
			if (uMsg == WM_NCCREATE)
			{
				CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
				pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

				pThis->m_hwnd = hwnd;
			}
			else
			{
				pThis = AppState(hwnd);
			}

			if(pThis)
			{
				return pThis->HandleMessage(uMsg, wParam, lParam);
			}
			else
			{
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}

		Window() : m_hwnd(NULL) {}

		BOOL Create(WindowCreateSettings settings)
		{
			WNDCLASS wc = { 0 };
			wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
			wc.hInstance = settings.hInstance;
			wc.lpszClassName = ClassName();

			RegisterClass(&wc);

			m_hwnd = CreateWindowEx(
				settings.dwExStyle, ClassName(), settings.lpWindowName, settings.dwStyle, settings.x, settings.y,
				settings.nWidth, settings.nHeight, settings.hwndParent, settings.hMenu, settings.hInstance, this);

			return (m_hwnd ? TRUE : FALSE);
		}

		inline static DERIVED_TYPE* AppState(HWND hwnd)
		{
			DERIVED_TYPE* pThis = reinterpret_cast<DERIVED_TYPE*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			return pThis;
		}

		inline HWND WindowHandle() const { return m_hwnd; }
		inline HINSTANCE ApplicationInstance() const { return m_hInstance; }
	protected:
		virtual LPCWSTR ClassName() const = 0;
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	};

}

#endif // !_PROTO_WINDOW_


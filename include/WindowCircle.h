#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "Window.h"

namespace ProtoText
{
	class WindowCircle : public ProtoText::Window<WindowCircle>
	{
	private:
		ID2D1Factory *pFactory;
		ID2D1HwndRenderTarget *pRenderTarget;
		ID2D1SolidColorBrush *pBrush;
		D2D1_ELLIPSE ellipse;

		void CalculateLayout();
		HRESULT CreateGraphicsResources();
		void DiscardGraphicsResources();
		void OnPaint();
		void Resize();

	public:
		WindowCircle() : pFactory(nullptr), pRenderTarget(nullptr), pBrush(nullptr)
		{

		}

		LPCWSTR ClassName() const override;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
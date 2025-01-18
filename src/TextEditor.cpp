#include "TextEditor.h"

ProtoText::TextEditor::TextEditor()
	:mp_dWriteFactory(nullptr), mp_dWriteTextFormat(nullptr), mp_d2d1Factory(nullptr),
	mp_hwndRenderTarget(nullptr), mp_blackBrush(nullptr), mp_text(nullptr), m_textLength(0)
{

}

HRESULT ProtoText::TextEditor::Initialize()
{
	HWND hwndTextEditor = TextEditor::WindowHandle();

	/*
		DEVICE INDEPENDENT RESOURCES
	*/

	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mp_d2d1Factory);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&mp_dWriteFactory));

		mp_text = L"Hello World ! This is Proto Text";
		m_textLength = (UINT32)wcslen(mp_text);

		FLOAT DPI = 96.0f;
		FLOAT dpiX, dpiY;
		mp_d2d1Factory->GetDesktopDpi(&dpiX, &dpiY);

		FLOAT dpiScaleX = dpiX / DPI;
		FLOAT dpiScaleY = dpiY / DPI;

		hr = mp_dWriteFactory->CreateTextFormat(
			L"Gabriola",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			12.0f * dpiScaleX * dpiScaleY,
			L"en-us",
			&mp_dWriteTextFormat
		);

		mp_dWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		mp_dWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	/*
		DEVICE DEPENDENT RESOURCES
	*/

	RECT rc;
	GetClientRect(hwndTextEditor, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	if (!mp_hwndRenderTarget)
	{
		hr = mp_d2d1Factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), 
			D2D1::HwndRenderTargetProperties(hwndTextEditor,size), &mp_hwndRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = mp_hwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &mp_blackBrush);
		}
	}

	return hr;
}

HRESULT ProtoText::TextEditor::OnResize()
{
	HWND hwndTextEditor = TextEditor::WindowHandle();

	RECT rc;
	GetClientRect(hwndTextEditor, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	HRESULT hr = mp_hwndRenderTarget->Resize(size);
	mp_hwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &mp_blackBrush);

	mp_dWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mp_dWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	InvalidateRect(hwndTextEditor, nullptr, TRUE);

	return hr;
}

void ProtoText::TextEditor::RenderText()
{
	HWND hwndTextEditor = TextEditor::WindowHandle();
	FLOAT dpiX, dpiY;
	mp_d2d1Factory->GetDesktopDpi(&dpiX, &dpiY);

	FLOAT dpiScaleX = dpiX / 96.0f;
	FLOAT dpiScaleY = dpiY / 96.0f;

	RECT rc;
	GetClientRect(hwndTextEditor, &rc);
	
	
	D2D1_RECT_F textAreaSize = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / dpiScaleX,
		static_cast<FLOAT>(rc.top) / dpiScaleY,
		static_cast<FLOAT>(rc.right - rc.left) / dpiScaleX,
		static_cast<FLOAT>(rc.bottom - rc.top) / dpiScaleY
	);
	
	mp_hwndRenderTarget->DrawText(mp_text, m_textLength, mp_dWriteTextFormat, textAreaSize, mp_blackBrush);
}

void ProtoText::TextEditor::CleanUp()
{
	SafeRelease(&mp_hwndRenderTarget);
	SafeRelease(&mp_blackBrush);
}


PCWSTR ProtoText::TextEditor::ClassName() const
{
	return L"TextEditor";
}

LRESULT ProtoText::TextEditor::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = TextEditor::WindowHandle();
	switch (uMsg)
	{
		case WM_CREATE:
		{
			Initialize();
			return 0;
		}

		case WM_PAINT:
		{
			
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
				
			mp_hwndRenderTarget->BeginDraw();
			mp_hwndRenderTarget->SetTransform(D2D1::IdentityMatrix());
			mp_hwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
				
			RenderText();
			mp_hwndRenderTarget->EndDraw();

			EndPaint(hwnd, &ps);
			
			return 0;
		}

		case WM_SIZE:
		{
			InvalidateRect(hwnd, nullptr, TRUE);
			OnResize();
			return 0;
		}

		case WM_DESTROY:
		{
			CleanUp();
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

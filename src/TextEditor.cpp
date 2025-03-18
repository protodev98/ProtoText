#include "TextEditor.h"

ProtoText::TextEditor::TextEditor():config(), text(), caret(), textProperties()
{
	
}

HRESULT ProtoText::TextEditor::Initialize()
{
	/*
		DEVICE INDEPENDENT RESOURCES
	*/

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&config.dWriteFactory));

	HRESULT hr = config.dWriteFactory->CreateTextFormat(
		L"Gabriola",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		textProperties.fontSize,
		L"en-us",
		&config.dWriteTextFormat
	);

	/*
		DEVICE DEPENDENT RESOURCES
	*/

	RECT rc;
	GetClientRect(m_hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	if (!config.hwndRenderTarget)
	{
		hr = config.d2d1Factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), 
			D2D1::HwndRenderTargetProperties(m_hwnd,size), &config.hwndRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = config.hwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &config.blackBrush);
		}
	}

	//Initialize Caret X
	caret.bottomX = 1.0f;
	caret.topX = 1.0f;

	return hr;
}

HRESULT ProtoText::TextEditor::OnResize()
{
	HWND hwndTextEditor = TextEditor::WindowHandle();

	RECT rc;
	GetClientRect(hwndTextEditor, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	HRESULT hr = config.hwndRenderTarget->Resize(size);

	return hr;
}

void ProtoText::TextEditor::RenderText()
{
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	D2D1_RECT_F textAreaSize = D2D1::RectF(rc.left,rc.top,rc.right,rc.bottom);

	FLOAT maxWidth = textAreaSize.right - textAreaSize.left;
	FLOAT maxHeight = textAreaSize.bottom - textAreaSize.top;

	const WCHAR* textString = text.c_str();
	UINT32 textLength = text.length();
	HRESULT hr = config.dWriteFactory->CreateTextLayout(textString, textLength, config.dWriteTextFormat, maxWidth, maxHeight, &config.textLayout);

	config.textLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, textProperties.lineSpacing, textProperties.baseline);
	if (SUCCEEDED(hr))
	{
		/*
			CARET - BEGIN
		*/

		//Determine X components
		DWRITE_HIT_TEST_METRICS hitMetrics;
		float caretX = 0.0f, caretY = 0.0f;

		HRESULT hr = config.textLayout->HitTestTextPosition(
			text.length(),
			TRUE,
			&caretX,
			&caretY,
			&hitMetrics
		);

		caret.topX = caretX;
		caret.bottomX = caretX;

		//Determine Y components
		std::vector<DWRITE_LINE_METRICS> lineMetrics;
		UINT32 metricLineCount = 1;

		/* This call is to determine the line count accurately to allocate a buffer;
		* We use this to allocate a buffer and pass it to the second call, which returns the metrics we want
		*/
		config.textLayout->GetLineMetrics(nullptr, metricLineCount, &metricLineCount);

		lineMetrics.resize(metricLineCount);
		config.textLayout->GetLineMetrics(lineMetrics.data(), metricLineCount, &metricLineCount);

		UINT32 multiple = (metricLineCount == 0 ? 1 : metricLineCount);

		caret.bottomY = textProperties.baseline + (multiple - 1) * textProperties.lineSpacing;
		caret.topY = caret.bottomY - textProperties.baseline;

		/*
			CARET - END
		*/

		config.hwndRenderTarget->DrawTextLayout(D2D1::Point2F(rc.left, rc.top), config.textLayout, config.blackBrush);
	}
}

void ProtoText::TextEditor::OnKeyPress(WPARAM wParam)
{
	WCHAR character = (WCHAR)wParam;

	switch (wParam)
	{
		//Backspace
		case VK_BACK:
		{
			if (!text.empty())
			{
				text.pop_back();
			}

			break;
		}

		//Enter Key
		case VK_RETURN:
		{
			text += character;

			caret.topX = 1.0f;
			caret.bottomX = 1.0f;

			break;
		}

		//Every other keys
		default:
		{
			text += character;
			break;
		}

	}
}

void ProtoText::TextEditor::CleanUp()
{
	SafeRelease(&config.d2d1Factory);
	SafeRelease(&config.dWriteFactory);
	SafeRelease(&config.hwndRenderTarget);
	SafeRelease(&config.dWriteTextFormat);
	SafeRelease(&config.textLayout);
	SafeRelease(&config.blackBrush);
	KillTimer(m_hwnd, TIMER);
}


PCWSTR ProtoText::TextEditor::ClassName() const
{
	return L"TextEditor";
}

LRESULT ProtoText::TextEditor::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			//Created this here instead of inside Intialize() because we need it to call GetDesktopDpi()
			HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &config.d2d1Factory);
			
			if (SUCCEEDED(hr))
			{
				FLOAT dpiX, dpiY;
				config.d2d1Factory->GetDesktopDpi(&dpiX, &dpiY);

				FLOAT dpiScaleX = dpiX / USER_DEFAULT_SCREEN_DPI;
				//dpiScaleY = dpiY / USER_DEFAULT_SCREEN_DPI;

				textProperties.Multiplier(dpiScaleX);

				Initialize();

				//500ms - Triggers WM_TIMER => For blinking custom Caret
				SetTimer(m_hwnd, TIMER, 500, (TIMERPROC) nullptr);
			}

			return 0;
		}

		case WM_PAINT:
		{
			POINT point = { 0, 0 };
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hwnd, &ps);

			config.hwndRenderTarget->BeginDraw();
			config.hwndRenderTarget->SetTransform(D2D1::IdentityMatrix());
			config.hwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
				
			RenderText();

			if (caret.isCaretVisible)
			{
				config.hwndRenderTarget->DrawLine(D2D1::Point2F(caret.topX, caret.topY), D2D1::Point2F(caret.bottomX, caret.bottomY),
					config.blackBrush, caret.strokeWidth);
			}
			
			config.hwndRenderTarget->EndDraw();

			EndPaint(m_hwnd, &ps);
			
			return 0;
		}
		
		//Keycodes : https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		case WM_CHAR:
		{
			OnKeyPress(wParam);
			InvalidateRect(m_hwnd, nullptr, true);
			return 0;
		}

		case WM_TIMER:
		{
			/* Invalidate Caret - For blinking */
			bool isCaretVisible = caret.isCaretVisible;
			caret.isCaretVisible = !isCaretVisible;

			FLOAT caretCorrection = caret.strokeWidth;

			RECT caretArea;
			caretArea.left = caret.topX - caretCorrection; caretArea.top = caret.topY - caretCorrection;
			caretArea.right = caret.bottomX + caretCorrection; caretArea.bottom = caret.bottomY + caretCorrection;

			InvalidateRect(m_hwnd, &caretArea, true);

			return 0;
		}

		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(m_hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
			config.hwndRenderTarget->Resize(size);
			return 0;
		}

		case WM_DESTROY:
		{
			CleanUp();
			return 0;
		}
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

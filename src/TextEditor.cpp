#include "TextEditor.h"

ProtoText::TextEditor::TextEditor():config(), text(), lineCount(1), caret(), textProperties(), dpiScaleX(1.0f), dpiScaleY(1.0f)
{
	
}

HRESULT ProtoText::TextEditor::Initialize()
{
	/*
		DEVICE INDEPENDENT RESOURCES
	*/
	//Already initialized 'd2d1Factory' and 'dWriteFactory' in WM_CREATE

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

	/*
	DWRITE_TEXT_METRICS textMetrics;
	config.textLayout->GetMetrics(&textMetrics);

	HDC hdc = GetDC(m_hwnd);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	CreateCaret(m_hwnd, nullptr, tm.tmAveCharWidth, tm.tmHeight);
	RECT editorRect;
	GetClientRect(m_hwnd, &editorRect);

	FLOAT caretCorrection = textProperties.fontSize;
	SetCaretPos(editorRect.left, editorRect.top + caretCorrection);
	ShowCaret(m_hwnd);
	*/

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
	if (SUCCEEDED(hr) && lineCount)
	{
		/*
		* 
		* 
		* 
		* 
		* 
				FIX THIS
		
		
		
		
		std::vector<DWRITE_LINE_METRICS> lineMetrics;
		UINT32 metricLineCount = 1;

		lineMetrics.resize(metricLineCount);
		config.textLayout->GetLineMetrics(lineMetrics.data(), metricLineCount, &metricLineCount);

		//CARET
		UINT32 multiple = (metricLineCount == 0 ? 1 : metricLineCount);
		FLOAT correction = ((textProperties.baseline - textProperties.lineSpacing) * multiple);
		
		caret.topX = 1.0f;
		caret.topY = (textProperties.fontSize - correction/2);

		caret.bottomY = caret.topY + textProperties.fontSize;
		caret.bottomX = 1.0f;
		*/

		std::vector<DWRITE_LINE_METRICS> lineMetrics;
		UINT32 metricLineCount = 1;
		config.textLayout->GetLineMetrics(nullptr, metricLineCount, &metricLineCount);

		lineMetrics.resize(metricLineCount);
		config.textLayout->GetLineMetrics(lineMetrics.data(), metricLineCount, &metricLineCount);

		//CARET
		UINT32 multiple = (metricLineCount == 0 ? 1 : metricLineCount);

		caret.bottomY = textProperties.baseline + (multiple - 1) * textProperties.lineSpacing;
		caret.bottomX = 1.0f;

		caret.topX = 1.0f;
		caret.topY = caret.bottomY - textProperties.baseline;

		config.hwndRenderTarget->DrawTextLayout(D2D1::Point2F(rc.left, rc.top), config.textLayout, config.blackBrush);
	}
}

void ProtoText::TextEditor::OnKeyPress(WPARAM wParam)
{
	WCHAR character = (WCHAR)wParam;

	POINT caretPoint;
	GetCaretPos(&caretPoint);

	FLOAT caretCorrection = 12.0f * dpiScaleX;

	//if backspace
	if (wParam == VK_BACK && !text.empty())
	{
		text.pop_back();
		size_t caretPosition = text.length(); // Caret at the end of the string

		DWRITE_HIT_TEST_METRICS hitMetrics;
		float caretX = 0.0f, caretY = 0.0f;

		// Calculate the caret's precise position
		HRESULT hr = config.textLayout->HitTestTextPosition(
			caretPosition, // Index of the character to align the caret
			FALSE,         // FALSE = leading edge; TRUE = trailing edge
			&caretX,
			&caretY,
			&hitMetrics
		);


		SetCaretPos(caretX * dpiScaleX, caretY + caretCorrection);
	}
	else if (wParam == VK_RETURN)
	{
		text += character;
		size_t caretPosition = text.length(); // Caret at the end of the string

		DWRITE_HIT_TEST_METRICS hitMetrics;
		float caretX = 0.0f, caretY = 0.0f;

		// Calculate the caret's precise position
		HRESULT hr = config.textLayout->HitTestTextPosition(
			caretPosition, // Index of the character to align the caret
			FALSE,         // FALSE = leading edge; TRUE = trailing edge
			&caretX,
			&caretY,
			&hitMetrics
		);

		SetCaretPos(caretX * dpiScaleX, caretPoint.y + (12.0f * dpiScaleX));
	}
	else
	{
		text += character;
		size_t caretPosition = text.length(); // Caret at the end of the string

		DWRITE_HIT_TEST_METRICS hitMetrics;
		float caretX = 0.0f, caretY = 0.0f;

		// Calculate the caret's precise position
		HRESULT hr = config.textLayout->HitTestTextPosition(
			caretPosition, // Index of the character to align the caret
			FALSE,         // FALSE = leading edge; TRUE = trailing edge
			&caretX,
			&caretY,
			&hitMetrics
		);

		SetCaretPos(caretX * dpiScaleX, caretY + caretCorrection);
	}
}

void ProtoText::TextEditor::UpdateCaret()
{
	/*
	//For default configuration - pass negative values when called
	if (x > 0 && y > 0)
		SetCaretPos(x, y);

	POINT point = { 0, 0 };
	DWRITE_TEXT_METRICS textMetrics;
	config.textLayout->GetMetrics(&textMetrics);
	
	GetCaretPos(&point);

	//This for one line
	SetCaretPos(textMetrics.widthIncludingTrailingWhitespace * dpiScaleX, point.y); //Muliply with dpi made it work
	*/
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
	HDC hdc = GetDC(m_hwnd);

	RECT rc;
	GetClientRect(m_hwnd, &rc);

	switch (uMsg)
	{
		case WM_CREATE:
		{
			HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &config.d2d1Factory);
			
			if (SUCCEEDED(hr))
			{
				hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
					reinterpret_cast<IUnknown**>(&config.dWriteFactory));
			}

			FLOAT dpiX, dpiY;
			config.d2d1Factory->GetDesktopDpi(&dpiX, &dpiY);
			
			dpiScaleX = dpiX / USER_DEFAULT_SCREEN_DPI;
			dpiScaleY = dpiY / USER_DEFAULT_SCREEN_DPI;
			
			textProperties.Multiplier(dpiScaleX);

			Initialize();
			SetTimer(m_hwnd, TIMER, 500, (TIMERPROC) nullptr);
			
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
			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
			config.hwndRenderTarget->Resize(size);
			//OnResize();
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

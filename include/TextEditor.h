#ifndef _PROTO_TEXT_EDITOR_
#define _PROTO_TEXT_EDITOR_

#include <windows.h>
#include "Window.h"
#include <d2d1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <string>
#include "UtilFunctions.h"
#include <vector>

#define TIMER 1001

namespace ProtoText
{
	struct D2D1Config
	{
	public:
		ID2D1Factory* d2d1Factory;
		IDWriteFactory* dWriteFactory;
		IDWriteTextFormat* dWriteTextFormat;
		ID2D1HwndRenderTarget* hwndRenderTarget;
		ID2D1SolidColorBrush* blackBrush;
		IDWriteTextLayout* textLayout;

		D2D1Config() : d2d1Factory(nullptr), dWriteFactory(nullptr), dWriteTextFormat(nullptr),
			hwndRenderTarget(nullptr), blackBrush(nullptr), textLayout(nullptr)
		{

		}
	};

	struct TextProperties
	{
	public:
		FLOAT fontSize;
		FLOAT baseline;
		FLOAT lineSpacing;

		TextProperties(): fontSize(15.0f), baseline(12.0f), lineSpacing(20.0f)
		{

		}

		TextProperties(FLOAT fs, FLOAT bl, FLOAT ls) : fontSize(fs), baseline(bl), lineSpacing(ls)
		{

		}

		void Multiplier(FLOAT multiplier)
		{
			fontSize *= multiplier;
			lineSpacing *= multiplier;
			baseline *= multiplier;
		}
	};

	struct Caret
	{
	public:
		FLOAT topX, topY;
		FLOAT bottomX, bottomY;
		FLOAT strokeWidth;
		UINT currentLine;
		bool isCaretVisible;

		Caret(): topX(0.0f), topY(0.0f), bottomX(0.0f), bottomY(0.0f), strokeWidth(1.5f), currentLine(1), isCaretVisible(true)
		{

		}
	};

	class TextEditor : public ProtoText::Window<TextEditor>
	{
	private:
		std::wstring text;
		TextProperties textProperties;
		Caret caret;
		D2D1Config config;
		UINT32 lineCount;

		FLOAT dpiScaleX, dpiScaleY;

		HRESULT Initialize();
		HRESULT OnResize();
		void CleanUp();
		void RenderText();
		void OnKeyPress(WPARAM wParam);
		void UpdateCaret();

	public:
		TextEditor();
		PCWSTR  ClassName() const;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

#endif //!_PROTO_TEXT_EDITOR_

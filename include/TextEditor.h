#ifndef _PROTO_TEXT_EDITOR_
#define _PROTO_TEXT_EDITOR_

#include <windows.h>
#include "Window.h"
#include <d2d1.h>
#include <dwrite.h>

namespace ProtoText
{
	class TextEditor : public ProtoText::Window<TextEditor>
	{
	private:
		IDWriteFactory* mp_dWriteFactory;
		IDWriteTextFormat* mp_dWriteTextFormat;
		WCHAR* mp_text;
		UINT32 m_textLength;

		ID2D1Factory* mp_d2d1Factory;
		ID2D1HwndRenderTarget* mp_hwndRenderTarget;
		ID2D1SolidColorBrush* mp_blackBrush;

		HRESULT Initialize();
		HRESULT OnResize();
		void CleanUp();
		void RenderText();
	public:
		TextEditor();
		PCWSTR  ClassName() const;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

#endif //!_PROTO_TEXT_EDITOR_

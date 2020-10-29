#pragma once
#include "RISERWindow.h"
#include "RISERKeyboard/RISERKeyboardClass.h"
#include "RISERMouse/RISERMouseClass.h"
#include "RISERGraphics/RISERGraphics.h"

class RISERWindowContainer
{
public:
	RISERWindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RISERWindow window;
	RISERKeyboardClass keyboard;
	RISERMouseClass mouse;
	RISERGraphics gfx;
};

#include "RISEREngine.h"

bool RISEREngine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
{
	if (!this->window.Init(this, hInstance, windowTitle, windowClass, width, height))
		return false;
	if (!gfx.Init(this->window.GetHWND(), width, height))
		return false;

	return true;
}

bool RISEREngine::ProcessWindowMessages()
{
	return this->window.ProcessWindowMessages();
}

void RISEREngine::Update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char c = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		RISERKeyboardEvent ke = keyboard.ReadKey();
		unsigned char keycode = ke.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		RISERMouseEvent m = mouse.ReadEvent();
	}
}

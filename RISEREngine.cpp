#include "RISEREngine.h"

bool RISEREngine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
{
	return this->window.Init(this,hInstance, windowTitle, windowClass, width, height);
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
		if (m.GetType() == RISERMouseEvent::EventType::RAW_MOVE)
		{
			std::string s = "X: ";
			s += std::to_string(m.GetPosX());
			s += ", ";
			s += "Y: ";
			s += std::to_string(m.GetPosY());
			s += "\n";
			OutputDebugStringA(s.c_str());
		}
	}
}

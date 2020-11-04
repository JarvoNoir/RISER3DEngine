#include "RISEREngine.h"

bool RISEREngine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
{
	timer.Start();
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
	float deltaTime = timer.GetMillisecondsElapsed();
	timer.Restart();

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
		if (mouse.IsRightDown())
		{
			if (m.GetType() == RISERMouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)m.GetPosY() * 0.01f, (float)m.GetPosX() * 0.01f, 0);
			}
		}
	}

	const float cameraSpeed = 0.06f;
	//camera movement
	if (keyboard.KeyIsPressed('W'))
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * deltaTime);
	if (keyboard.KeyIsPressed('S'))
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * deltaTime);
	if (keyboard.KeyIsPressed('A'))
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * deltaTime);
	if (keyboard.KeyIsPressed('D'))
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * deltaTime);
	if (keyboard.KeyIsPressed(VK_SPACE))
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed * deltaTime, 0.0f);
	if (keyboard.KeyIsPressed('Z'))
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed * deltaTime, 0.0f);
}

void RISEREngine::RenderFrame()
{
	this->gfx.RenderFrame();
}

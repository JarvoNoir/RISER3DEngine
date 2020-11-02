#pragma once
#include "RISERWindowContainer.h"
#include "RISERTimer.h"

class RISEREngine : RISERWindowContainer
{
public:
	bool Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessWindowMessages();
	void Update();
	void RenderFrame();
private:
	RISERTimer timer;
};
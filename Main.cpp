#include "RISEREngine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	RISEREngine engine;
	if (engine.Init(hInstance, "RISER3D", "RISERWindowClass", 800, 600))
	{
		while (engine.ProcessWindowMessages())
		{
			engine.Update();
			engine.RenderFrame();
		}
	}

	return 0;
}
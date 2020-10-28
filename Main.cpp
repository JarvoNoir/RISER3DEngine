#include "RISEREngine.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	RISEREngine engine;
	engine.Init(hInstance, "Title", "WINDOWWWWW", 800, 600);
	while (engine.ProcessWindowMessages())
	{
		engine.Update();
	}

	return 0;
}
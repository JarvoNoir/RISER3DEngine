#include "RISEREngine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	RISEREngine engine;
	if (engine.Init(hInstance, "RISER3D", "RISERWindowClass", 800, 600))
	{
		while (engine.ProcessWindowMessages()==true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}

	//Release ImGui references
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
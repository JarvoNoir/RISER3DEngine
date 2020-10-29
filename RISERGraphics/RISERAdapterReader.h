#pragma once
#include "..\\RISERErrorLogger.h"
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib,"DXGI.lib")
#include <wrl/client.h>
#include <vector>

class RISERAdapterData
{
public:
	RISERAdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class RISERAdapterReader
{
public:
	static std::vector<RISERAdapterData> GetAdapters();
private:
	static std::vector<RISERAdapterData> adapters;
};
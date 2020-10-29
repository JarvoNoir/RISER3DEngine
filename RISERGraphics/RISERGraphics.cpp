#include "RISERGraphics.h"

bool RISERGraphics::Init(HWND hwnd, int width, int height)
{
	if(!InitDirectX(hwnd,width,height))
		return false;
	return true;
}

void RISERGraphics::RenderFrame()
{
	float bgColor[] = { 0.0f ,1.0f ,1.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->swapChain->Present(1, NULL);
}

bool RISERGraphics::InitDirectX(HWND hwnd, int width, int height)
{
	std::vector<RISERAdapterData> adapters = RISERAdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		RISERErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}
	//set up swapchain
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//create swapchain
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter,//IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, //For software driver type
		NULL, //flags for runtime layers
		NULL, //feature levels array
		0, //number of feature levels in array
		D3D11_SDK_VERSION,
		&scd,//swapchain desc
		this->swapChain.GetAddressOf(), //swapchain address
		this->device.GetAddressOf(), //device address
		NULL, //supported feature level
		this->deviceContext.GetAddressOf() //device context address
	);

	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	//set up back buffer
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	//create render view
	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	return true;
}

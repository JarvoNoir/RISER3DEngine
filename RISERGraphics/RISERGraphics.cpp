#include "RISERGraphics.h"

bool RISERGraphics::Init(HWND hwnd, int width, int height)
{
	if(!InitDirectX(hwnd,width,height))
		return false;
	if (!InitShaders())
		return false;
	if (!InitScene())
		return false;

	return true;
}

void RISERGraphics::RenderFrame()
{
	float bgColor[] = { 0.0f ,0.0f ,0.0f, 1.0f };
	//clear
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	//draw
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	UINT stride = sizeof(RISERVertex); //size of the input for the buffer
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(3, 0);
	//present
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
	//set output merger
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	//Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	//define viewport properties
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	//set viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	return true;
}

bool RISERGraphics::InitShaders()
{
	std::wstring shaderFolder = L"";
	//m
#pragma region SetShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG
	#ifdef _WIN64 //x64
			shaderFolder = L"..\\RISER3DEngine\\x64\\Debug\\";
	#else	//x86
			shaderFolder = L"..\\RISER3DEngine\\Debug\\";
	#endif 
#else
	#ifdef _WIN64
			shaderFolder = L"..\\RISER3DEngine\\x64\\Release\\";
	#else //x86
			shaderFolder = L"..\\RISER3DEngine\\Release\\";
	#endif
#endif
	}
	//define  input layout for shader
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.Init(this->device, shaderFolder + L"RISERVertexShader.cso",layout,numElements))
		return false;

	if (!pixelShader.Init(this->device, shaderFolder + L"RISERPixelShader.cso"))
		return false;

	return true;
}

bool RISERGraphics::InitScene()
{
	//create an arrary of RISER vertices
	RISERVertex v[] =
	{
		RISERVertex(0.0f,-0.1f),//c
		RISERVertex(-0.1f,0.0f),//l
		RISERVertex(0.1f,0.0f)//r
	};

	//create description for vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(RISERVertex) * ARRAYSIZE(v); //multiply by array size in case further verts are added later
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	//create subresource data
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	return true;
}

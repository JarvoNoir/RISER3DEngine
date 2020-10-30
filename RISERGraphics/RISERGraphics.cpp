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
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//draw
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	UINT offset = 0;
	//draw square
	this->deviceContext->PSSetShaderResources(0, 1, this->texture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(indexBuffer.BufferSize(), 0, 0);
	//draw text
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"RISER3D Engine", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
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

	//set depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//create texture
	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	//create stencil view
	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	//set output merger
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	
	//create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	
	hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	//define viewport properties
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height; 
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//set viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//create rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	
	//set fill mode and turn on backface culling
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	
	//set rasterizer state
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	//init fonts
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms.spritefont");
	
	//create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // x coord
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // y coord
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // z coord
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}


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
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}

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
	//create an array of RISER vertices
	//square
	RISERVertex v[] =
	{
		//x,y,z, u,v
		RISERVertex(-0.5f, -0.5f, 1.0f, 0.0f, 1.0f),//bottom left
		RISERVertex(-0.5f,  0.5f, 1.0f, 0.0f, 0.0f),//top left
		RISERVertex( 0.5f,  0.5f, 1.0f, 1.0f, 0.0f),//top right
		RISERVertex( 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f),//bottom right
	};
	//initialise vertex buffer using verts
	HRESULT hr = this->vertexBuffer.Init(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}
	//set up indices
	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};
	//initialise index buffer using indices
	hr = this->indexBuffer.Init(this->device.Get(),indices,ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create index buffer.");
		return false;
	}

	//create texture from file
	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\Teams.png", nullptr, texture.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	return true;
}

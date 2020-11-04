#include "RISERGraphics.h"

bool RISERGraphics::Init(HWND hwnd, int width, int height)
{
	//set member variables
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if(!InitDirectX(hwnd))
		return false;
	if (!InitShaders())
		return false;
	if (!InitScene())
		return false;

	//set ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

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
	this->deviceContext->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	UINT offset = 0;
	static float alpha = 1.0f;
	//Update Constant Buffer
	{//pavement
		static float transOffset[3] = { 0.0f,0.0f,0.0f };
		XMMATRIX worldMatrix = XMMatrixTranslation(transOffset[0], transOffset[1], transOffset[2]);
		//set world matrix
		//XMMATRIX worldMatrix = XMMatrixIdentity();

		//set constant buffer matrix
		risercb_vs_vertexShader.data.matrix = worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		//transpose constant buffer matrix from row major to column major
		DirectX::XMMatrixTranspose(risercb_vs_vertexShader.data.matrix);
		if (!risercb_vs_vertexShader.ApplyChanges())
			return;
		//set vertex
		this->deviceContext->VSSetConstantBuffers(0, 1, this->risercb_vs_vertexShader.GetAddressOf());
		//set pixel
		this->risercb_ps_pixelShader.data.alpha = alpha;
		this->risercb_ps_pixelShader.ApplyChanges();
		this->deviceContext->PSSetConstantBuffers(0, 1, this->risercb_ps_pixelShader.GetAddressOf());
		//draw grass
		this->deviceContext->PSSetShaderResources(0, 1, this->pavementTexture.GetAddressOf());
		this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(indexBuffer.BufferSize(), 0, 0);
	}
	
	//draw fps
	static int frames = 0;
	static std::string fpsCounter = "FPS: 0";
	frames += 1;
	if (fpsTimer.GetMillisecondsElapsed() > 1000.0)
	{
		fpsCounter = "FPS: " + std::to_string(frames);
		frames = 0;
		fpsTimer.Restart();
	}
	//draw to screen	
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), RISERStringConverter::StringToWide(fpsCounter).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
	//ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//create window
	ImGui::Begin("RISER3D");
	ImGui::Text("RISER3d Engine UI is a go!");
	//ImGui::DragFloat3("Translation x/y/z", transOffset, 0.1f, -5.0f, 5.0f, 0, 0);
	ImGui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);
	ImGui::End();
	//assemble together draw data
	ImGui::Render();
	//render draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//present
	this->swapChain->Present(1, NULL);
}

bool RISERGraphics::InitDirectX(HWND hwnd)
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
	scd.BufferDesc.Width = this->windowWidth;
	scd.BufferDesc.Height = this->windowHeight;
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
	depthStencilDesc.Width = this->windowWidth;
	depthStencilDesc.Height = this->windowHeight;
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
	viewport.Width = this->windowWidth;
	viewport.Height = this->windowHeight; 
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

	//create blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	D3D11_RENDER_TARGET_BLEND_DESC renderBlendState;
	ZeroMemory(&renderBlendState, sizeof(renderBlendState));

	//set flags
	renderBlendState.BlendEnable = true;
	renderBlendState.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	renderBlendState.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	renderBlendState.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	renderBlendState.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	renderBlendState.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	renderBlendState.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	renderBlendState.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0] = renderBlendState;
	//set blend state
	hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		RISERErrorLogger::Log(hr, "Failed to create blend state.");
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
		RISERVertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
		RISERVertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
		RISERVertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
		RISERVertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
		RISERVertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
		RISERVertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
		RISERVertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
		RISERVertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
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
		0, 1, 2, //FRONT
		0, 2, 3, //FRONT
		4, 7, 6, //BACK 
		4, 6, 5, //BACK
		3, 2, 6, //RIGHT SIDE
		3, 6, 7, //RIGHT SIDE
		4, 5, 1, //LEFT SIDE
		4, 1, 0, //LEFT SIDE
		1, 5, 6, //TOP
		1, 6, 2, //TOP
		0, 3, 7, //BOTTOM
		0, 7, 4, //BOTTOM
	};
	//initialise index buffer using indices
	hr = this->indexBuffer.Init(this->device.Get(),indices,ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create index buffer.");
		return false;
	}

	//create texture from file
	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	//init Constant Buffer(s)
	hr = this->risercb_vs_vertexShader.Init(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to initialise constant buffer(risercb_vs_vertexShader).");
		return false;
	}

	hr = this->risercb_ps_pixelShader.Init(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to initialise constant buffer(risercb_ps_pixelShader).");
		return false;
	}

	//init camera
	this->camera.SetPosition(0.0f, 0.0f, -2.0f);
	this->camera.SetProjectionValues(90.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 1000.0f);

	return true;
}

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
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	UINT offset = 0;
	{
		this->model.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
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
	//ImGui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);
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
	try
	{
		std::vector<RISERAdapterData> adapters = RISERAdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			RISERErrorLogger::Log("No DXGI Adapters found.");
			return false;
		}
		//set up swapchain
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
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
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		//set up back buffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		RISERCOM_ERROR_IF_FAILED(hr, "GetBuffer() failed.");

		//create render view
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create render target view");

		//set depth/stencil buffer
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		//create texture
		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		//create stencil view
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		//set output merger
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc(D3D11_DEFAULT);
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//Create viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
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
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//create blend state
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC renderBlendState = { 0 };

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
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create blend state.");
		//init fonts
		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms.spritefont");

		//create sampler state
		CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
		//D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // x coord
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // y coord
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // z coord
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (RISERCOMException& exception)
	{
		RISERErrorLogger::Log(exception);
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
	try
	{
		//load texture(s) from file
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//init Constant Buffer(s)
		hr = this->risercb_vs_vertexShader.Init(this->device.Get(), this->deviceContext.Get());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialise constant buffer(risercb_vs_vertexShader).");

		hr = this->risercb_ps_pixelShader.Init(this->device.Get(), this->deviceContext.Get());
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialise constant buffer(risercb_ps_pixelShader).");
		//init model(s)
		if (!model.Init(this->device.Get(), this->deviceContext.Get(), this->pavementTexture.Get(), risercb_vs_vertexShader))
			return false;
		//init camera
		this->camera.SetPosition(0.0f, 0.0f, -2.0f);
		this->camera.SetProjectionValues(90.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 1000.0f);
	}
	catch(RISERCOMException& exception)
	{
		RISERErrorLogger::Log(exception);
		return false;
	}
	return true;
}

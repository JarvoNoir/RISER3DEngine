#pragma once
#include "RISERAdapterReader.h"
#include "RISERShaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include"RISERCamera.h"
#include "..\\RISERTimer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "RISERModel.h"

class RISERGraphics
{
public:
	bool Init(HWND hwnd, int width, int height);
	void RenderFrame();
	//Camera
	RISERCamera camera;
	//model
	RISERModel model;
private:
	bool InitDirectX(HWND hwnd);
	bool InitShaders();
	bool InitScene();
	//device setup
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	//shaders
	RISERVertexShader vertexShader;
	RISERPixelShader pixelShader;
	//buffers
	RISERConstantBuffer<RISERCB_VS_VertexShader> risercb_vs_vertexShader;
	RISERConstantBuffer<RISERCB_PS_PixelShader> risercb_ps_pixelShader;
	//rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	//blend state
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	//depth stencil view + buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	//fonts
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
	//sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;
	//window variables
	int windowWidth = 0;
	int windowHeight = 0;
	//timer
	RISERTimer fpsTimer;
};
#pragma once
#include "RISERAdapterReader.h"
#include "RISERShaders.h"
#include "RISERVertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "RISERVertexBuffer.h"
#include "RISERIndexBuffer.h"
#include "RISERConstBufferTypes.h"
#include "RISERConstantBuffer.h"
#include"RISERCamera.h"

class RISERGraphics
{
public:
	bool Init(HWND hwnd, int width, int height);
	void RenderFrame();
	//Camera
	RISERCamera camera;
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
	RISERVertexBuffer<RISERVertex> vertexBuffer;
	RISERIndexBuffer indexBuffer;
	RISERConstantBuffer<RISERCB_VS_VertexShader> constantBuffer;
	//rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	//depth stencil view + buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	//fonts
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
	//sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	//window variables
	int windowWidth = 0;
	int windowHeight = 0;
};
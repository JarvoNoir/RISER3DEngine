#pragma once
#include "RISERAdapterReader.h"
#include "RISERShaders.h"
#include "RISERVertex.h"

class RISERGraphics
{
public:
	bool Init(HWND hwnd, int width, int height);
	void RenderFrame();
private:
	bool InitDirectX(HWND hwnd, int width, int height);
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
	//vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer2;
	//rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	//depth stencil view + buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
};
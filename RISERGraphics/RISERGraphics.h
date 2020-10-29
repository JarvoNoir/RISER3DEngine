#pragma once
#include "RISERAdapterReader.h"
#include "RISERShaders.h"

class RISERGraphics
{
public:
	bool Init(HWND hwnd, int width, int height);
	void RenderFrame();
private:
	bool InitDirectX(HWND hwnd, int width, int height);
	bool InitShaders();
	//device setup
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	//shaders
	RISERVertexShader vertexShader;
	RISERPixelShader pixelShader;
};
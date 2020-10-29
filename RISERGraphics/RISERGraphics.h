#pragma once
#include "RISERAdapterReader.h"

class RISERGraphics
{
public:
	bool Init(HWND hwnd, int width, int height);
private:
	bool InitDirectX(HWND hwnd, int width, int height);
	
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};
#pragma once
#include <DirectXMath.h>

struct RISERCB_VS_VertexShader
{
	DirectX::XMMATRIX matrix;
};

struct RISERCB_PS_PixelShader
{
	float alpha = 1.0f;
};
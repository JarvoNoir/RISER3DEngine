#pragma once
#include <DirectXMath.h>

struct RISERVertex
{
	RISERVertex(){}
	RISERVertex(float x, float y, float r, float g, float b)
		: pos(x,y), colour(r,g,b){}

	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT3 colour;
};
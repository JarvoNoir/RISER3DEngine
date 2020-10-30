#pragma once
#include <DirectXMath.h>

struct RISERVertex
{
	RISERVertex(){}
	RISERVertex(float x, float y, float z, float r, float g, float b)
		: pos(x,y,z), colour(r,g,b){}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 colour;
};
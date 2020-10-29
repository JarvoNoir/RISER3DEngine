#pragma once
#include <DirectXMath.h>

struct RISERVertex
{
	RISERVertex(){}
	RISERVertex(float x, float y)
		: pos(x,y){}

	DirectX::XMFLOAT2 pos;
};
#pragma once
#include <DirectXMath.h>

struct RISERVertex
{
	RISERVertex(){}
	RISERVertex(float x, float y, float z, float u, float v)
		: pos(x,y,z), texCoord(u,v){}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};
#pragma once
#include "RISERVertex.h"
#include "RISERVertexBuffer.h"
#include "RISERIndexBuffer.h"
#include "RISERConstantBuffer.h"

using namespace DirectX;

class RISERModel
{
public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, RISERConstantBuffer<RISERCB_VS_VertexShader>& vertexShader);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX& viewProjectionMatrix);
private:
	void UpdateWorldMatrix();

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	//shaders
	RISERConstantBuffer<RISERCB_VS_VertexShader>* vertexShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;
	//buffers
	RISERVertexBuffer<RISERVertex> vertexBuffer;
	RISERIndexBuffer indexBuffer;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

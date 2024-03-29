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

	//movement functions
	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(const XMFLOAT3& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR& rot);
	void AdjustRotation(const XMFLOAT3& rot);
	void AdjustRotation(float x, float y, float z);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR& GetForwardVector();
	const XMVECTOR& GetRightVector();
	const XMVECTOR& GetBackwardVector();
	const XMVECTOR& GetLeftVector();
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
	//movement variables
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR forwardVector;
	XMVECTOR leftVector;
	XMVECTOR rightVector;
	XMVECTOR backwardVector;
};

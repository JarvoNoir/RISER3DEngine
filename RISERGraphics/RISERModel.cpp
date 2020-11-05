#include "RISERModel.h"

bool RISERModel::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, RISERConstantBuffer<RISERCB_VS_VertexShader>& vertexShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->vertexShader = &vertexShader;

	try
	{
		//Textured Square
		RISERVertex v[] =
		{
			RISERVertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			RISERVertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
			RISERVertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			RISERVertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			RISERVertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
			RISERVertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
			RISERVertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
			RISERVertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};

		//Load Vertex Data
		HRESULT hr = this->vertexBuffer.Init(this->device, v, ARRAYSIZE(v));
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");

		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};

		//Load Index Data
		hr = this->indexBuffer.Init(this->device, indices, ARRAYSIZE(indices));
		RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (RISERCOMException& exception)
	{
		RISERErrorLogger::Log(exception);
		return false;
	}

	this->UpdateWorldMatrix();
	return true;
}

void RISERModel::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void RISERModel::Draw(const XMMATRIX& viewProjectionMatrix)
{
	//Update Constant buffer with WVP Matrix
	this->vertexShader->data.matrix = this->worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->vertexShader->data.matrix = XMMatrixTranspose(this->vertexShader->data.matrix);
	this->vertexShader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->vertexShader->GetAddressOf());
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0); //Draw
}

void RISERModel::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}
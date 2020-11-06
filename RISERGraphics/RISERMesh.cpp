#include "RISERMesh.h"

RISERMesh::RISERMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<RISERVertex>& vertices, std::vector<DWORD>& indices)
{
	this->deviceContext = deviceContext;
	//vertices
	HRESULT hr = this->vertexBuffer.Init(device, vertices.data(), vertices.size());
	RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");
	//indices
	hr = this->indexBuffer.Init(device, indices.data(), indices.size());
}

RISERMesh::RISERMesh(const RISERMesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->vertexBuffer = mesh.vertexBuffer;
	this->indexBuffer = mesh.indexBuffer;
}

void RISERMesh::Draw()
{
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0);
}

#ifndef  RISERVertexBuffer_h_
#define RISERVertexBuffer_h_
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class RISERVertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::shared_ptr<UINT> stride;
	UINT bufferSize = 0;
public:
	RISERVertexBuffer(){}
	RISERVertexBuffer(const RISERVertexBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->bufferSize = rhs.bufferSize;
		this->stride = rhs.stride;
	}
	RISERVertexBuffer<T>& operator=(const RISERVertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->bufferSize = a.bufferSize;
		this->stride = a.stride;
		return *this;
	}

	ID3D11Buffer* Get() const { return buffer.Get();}
	ID3D11Buffer* const* GetAddressOf() const { return buffer.GetAddressOf();}
	UINT BufferSize() const { return this->bufferSize;}
	const UINT Stride() const { return *this->stride.get();}
	const UINT* StridePtr() const { return this->stride.get();}

	HRESULT Init(ID3D11Device* device, T* data, UINT numVertices)
	{
		//reset to prevent memory leaks
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->bufferSize = numVertices;
		//if stride is null, make stride
		if(this->stride.get()==nullptr)
			this->stride = std::make_shared<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}

};
#endif // ! RISERVertexBuffer_h_


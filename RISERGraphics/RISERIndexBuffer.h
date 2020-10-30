#ifndef RISERIndicesBuffer_h__
#define RISERIndicesBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class RISERIndexBuffer
{
private:
	RISERIndexBuffer(const RISERIndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;
public:
	RISERIndexBuffer() {}
	ID3D11Buffer* Get()const{return buffer.Get();}
	ID3D11Buffer* const* GetAddressOf()const{return buffer.GetAddressOf();}
	UINT BufferSize() const{return this->bufferSize;}

	HRESULT Init(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		this->bufferSize = numIndices;
		//Load Index Data
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
		return hr;
	}
};

#endif // RISERIndicesBuffer_h__

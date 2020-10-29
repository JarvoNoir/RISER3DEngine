#include "RISERShaders.h"

bool RISERVertexShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		RISERErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader: ";
		errorMsg += shaderpath;
		RISERErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11VertexShader* RISERVertexShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* RISERVertexShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}
#include "RISERShaders.h"

//vertex shader code
bool RISERVertexShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT numElements)
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

	hr = device->CreateInputLayout(inputLayoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create input layout.");
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

ID3D11InputLayout* RISERVertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}

//Pixel shader code
bool RISERPixelShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		RISERErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader: ";
		errorMsg += shaderpath;
		RISERErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader* RISERPixelShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* RISERPixelShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}
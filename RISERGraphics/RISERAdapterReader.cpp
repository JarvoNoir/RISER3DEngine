#include "RISERAdapterReader.h"

std::vector<RISERAdapterData> RISERAdapterReader::adapters;

std::vector<RISERAdapterData> RISERAdapterReader::GetAdapters()
{
	if (adapters.size() > 0) // if already init
		return adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	//create a DXGIFactory object
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		RISERErrorLogger::Log(hr, "Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;
	//if there is an adapter at the index
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		//push it into the vector and add one to the index
		adapters.push_back(RISERAdapterData(pAdapter));
		index++;
	}

	return adapters;
}

RISERAdapterData::RISERAdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);
	if (FAILED(hr))
		RISERErrorLogger::Log(hr, "Failed to get description for IXDGIAdapter.");
}

#include "RISERErrorLogger.h"
#include <comdef.h>

void RISERErrorLogger::Log(std::string err)
{
	std::string errorMessage = "Error: " + err;
	MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void RISERErrorLogger::Log(HRESULT hr,std::string err)
{
	_com_error error(hr);
	std::wstring errorMessage = L"Error: " + RISERStringConverter::StringToWide(err) + L"\n";
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}
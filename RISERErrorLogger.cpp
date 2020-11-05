#include "RISERErrorLogger.h"
#include <comdef.h>

void RISERErrorLogger::Log(std::string err)
{
	std::string errorMessage = "Error: " + err;
	MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void RISERErrorLogger::Log(HRESULT hr,std::string error)
{
	_com_error err(hr);
	std::wstring errorMessage = L"Error: " + RISERStringConverter::StringToWide(error) + L"\n";
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

void RISERErrorLogger::Log(HRESULT hr, std::wstring error)
{
	_com_error err(hr);
	std::wstring errorMessage = L"Error: " + error + L"\n " + err.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

void RISERErrorLogger::Log(RISERCOMException& exception)
{
	std::wstring errorMessage = exception.what();
	MessageBoxW(NULL, errorMessage.c_str(), L"ERROR", MB_ICONERROR);
}

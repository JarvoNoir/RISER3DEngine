#pragma once
#include <comdef.h>
#include "RISERStringConverter.h"

#define RISERCOM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw RISERCOMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )

class RISERCOMException
{
public:
	RISERCOMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + RISERStringConverter::StringToWide(std::string(msg)) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + RISERStringConverter::StringToWide(file);
		whatmsg += L"\nFunction: " + RISERStringConverter::StringToWide(function);
		whatmsg += L"\nLine: " + RISERStringConverter::StringToWide(std::to_string(line));
	}

	const wchar_t* what() const
	{
		return whatmsg.c_str();
	}
private:
	std::wstring whatmsg;
};
#include "RISERStringConverter.h"

std::wstring RISERStringConverter::StringToWide(std::string str)
{
	//convert a given string to a wide string
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}
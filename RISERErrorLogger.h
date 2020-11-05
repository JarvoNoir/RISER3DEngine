#pragma once
#include "RISERCOMException.h"
#include <Windows.h>

class RISERErrorLogger
{
public:
	static void Log(std::string error);
	static void Log(HRESULT hr, std::string error);
	static void Log(HRESULT hr, std::wstring error);
	static void Log(RISERCOMException& exception);
};
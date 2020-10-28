#pragma once
#include "RISERErrorLogger.h"

class RISERWindowContainer;

class RISERWindow
{
public:
	bool Init(RISERWindowContainer* pWindowContainer,HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessWindowMessages();
	~RISERWindow();
private:
	void RegisterWindow();
	HWND handle = NULL; //handle to this window class
	HINSTANCE hInstance = NULL; //handle to application instance
	std::string windowTitle = "";
	std::wstring windowTitleWide = L"";
	std::string windowClass = "";
	std::wstring windowClassWide = L"";
	int width = 0;
	int height = 0;
};
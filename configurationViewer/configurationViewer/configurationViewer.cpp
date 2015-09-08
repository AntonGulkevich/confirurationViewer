// configurationViewer.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

typedef Factory * (__stdcall *DLLGETFACTORY)(void);

int main(int argc, char* argv[])
{
	auto hInstDll = LoadLibrary(__T("win32DLib.dll"));
	if (!hInstDll) std::cout << ("Failed to load dll\n");
	auto pDllGetFactory = reinterpret_cast<DLLGETFACTORY>(GetProcAddress(hInstDll, "returnFactory"));
	auto pMyFactory = (pDllGetFactory)();
	if (pMyFactory == nullptr) return 0;
	auto testRegister = static_cast<CRegister *>(pMyFactory->CreateCRegister());
	system("PAUSE");

	return 0;
}
// AppDllTest.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"

typedef Factory * (__stdcall *DLLGETFACTORY)(void);

int main(int argc, char* argv[])
{
	//access to dll
	auto hInstDll = LoadLibrary(_T("win32DLib.dll"));
	if (!hInstDll) std::cout<<("Failed to load dll\n");
	auto pDllGetFactory = reinterpret_cast<DLLGETFACTORY>(GetProcAddress(hInstDll, "returnFactory"));
	auto pMyFactory = (pDllGetFactory)();
	if (pMyFactory == nullptr) return 0;
	//end of dll access

	auto drIoManager = static_cast<DriversIOManager *>(pMyFactory->CreateDriversIoManager());
	auto drManager = static_cast<DriverManager *>(pMyFactory->CreateDriverManager());
	auto testWorkManager = static_cast<WorkManager *>(pMyFactory->CreateWorkManager(drIoManager, drManager));
	auto testConfiguration = static_cast<CConfiguration *>(pMyFactory->CreateConfiguration(testWorkManager));

	testConfiguration->LoadConfig();

	WCHAR fileName[MAX_PATH];
	GetModuleFileName(nullptr, fileName, MAX_PATH);
	std::wstring str(fileName);
	std::string mystr(str.begin(), str.end());
	std::cout << mystr;
	system("PAUSE");

	return 0;
}


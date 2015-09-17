#include "stdafx.h"

typedef Factory * (__stdcall *DLLGETFACTORY)(void);
typedef void * (__stdcall *DLLRDMEM)(LPBYTE , DWORD , DWORD );//read
typedef void * (__stdcall *DLLWRMEM)(LPBYTE , DWORD , DWORD );//write

using std::cout;
using std::endl;
using std::string;
using std::wstring;

int main(int argc, char* argv[])
{
	//access to dll
	auto hInstDll = LoadLibrary(_T("win32DLib.dll"));
	if (!hInstDll) cout << ("Failed to load dll\n");
	else cout << "Dll loaded successfully\n";
	auto pDllGetFactory = reinterpret_cast<DLLGETFACTORY>(GetProcAddress(hInstDll, "returnFactory"));
	auto pMyFactory = (pDllGetFactory)();
	if (pMyFactory == nullptr) return 0;
	//end of dll access


	//auto drIoManager = static_cast<DriversIOManager *>(pMyFactory->CreateDriversIoManager());
	//auto drManager = static_cast<DriverManager *>(pMyFactory->CreateDriverManager());
	//auto testWorkManager = static_cast<WorkManager *>(pMyFactory->CreateWorkManager(drIoManager, drManager));
	//auto testConfiguration = static_cast<CConfiguration *>(pMyFactory->CreateConfiguration(testWorkManager));
	//testConfiguration->LoadConfig();
	auto pfnRdMem = reinterpret_cast<DLLRDMEM>(GetProcAddress(hInstDll, ("readSharedMem")));
	auto pfnWrMem = reinterpret_cast<DLLWRMEM>(GetProcAddress(hInstDll, ("writeSharedMem")));


	//void * objPtr = pfnRdMem();
	system("PAUSE");
	//better to free library. automatically free library when there is no process using it
	FreeLibrary(hInstDll);
	cout << "Free library" << endl;

	WCHAR fileName[MAX_PATH];
	GetModuleFileName(nullptr, fileName, MAX_PATH);
	wstring wstr(fileName);
	string str(wstr.begin(), wstr.end());
	cout << str << endl;
	system("PAUSE");

	
	

	return 0;
}



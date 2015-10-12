#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include <windows.h>
#include <iostream>
#include <memory.h>
#include <string>
#define EMULATION
#include "ucu_fw/src/dllapi/factory.h"



#define CLOCK_FILE_SIZE 64
#define EEPROMFILE_SIZE 128*1024

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

typedef Factory * (__stdcall *DLLGETFACTORY)(void);

using std::cout;
using std::endl;
using std::string;
using std::wstring;

bool isLittleEndian()
{
	short int number = 0x1;
	auto numPtr = reinterpret_cast<char*>(&number);
	return (numPtr[0] == 1);
}

inline bool isFileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

PCHAR createClock(const std::string& name)
{
	PBYTE buffer = new BYTE[CLOCK_FILE_SIZE];
	memset(buffer, 0xFF, CLOCK_FILE_SIZE);
	FILE *pFile;
	fopen_s(&pFile, name.c_str(), "w+b");
	fwrite(buffer, 1, CLOCK_FILE_SIZE, pFile);
	fclose(pFile);
	delete[] buffer;
	return "Clock-emulation file created.\n";
}

PCHAR createEeprom(const std::string& name)
{
	PBYTE buffer = new BYTE[EEPROMFILE_SIZE];
	memset(buffer, 0xFF, EEPROMFILE_SIZE);
	FILE *pFile;
	fopen_s(&pFile, name.c_str(), "w+b");
	fwrite(buffer, 1, EEPROMFILE_SIZE, pFile);
	fclose(pFile);
	delete[] buffer;
	return "EEPROM-emulation file created.\n";
}

bool validateConfiguration(const std::string &confFileName)
{

	return TRUE;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "RUS");
	//access to dll
	auto hInstDll = LoadLibrary(_T("win32DLib.dll"));
	if (!hInstDll) cout << ("Failed to load dll\n");
	else cout << "Dll loaded successfully\n";
	auto pDllGetFactory = reinterpret_cast<DLLGETFACTORY>(GetProcAddress(hInstDll, "returnFactory"));
	auto pMyFactory = (pDllGetFactory)();
	if (pMyFactory == nullptr) return 0;
	//end of dll access

	isLittleEndian() ? cout << "Little" : cout << "Big"; cout << "-endian machine." << endl;
	string fileNameToExist = "C:\\Users\\Gulkevich_A\\Desktop\\current\\ubs dll\\configViewer\\";

	isFileExists(fileNameToExist + "clock.bin") ? cout << "clock file exists" << endl : cout << createClock(fileNameToExist + "clock.bin") << endl;
	isFileExists(fileNameToExist + "eeprom.bin") ? cout << "eeprom file exists" << endl : cout << createEeprom(fileNameToExist + "eeprom.bin") << endl;


	const string commodToValidateFileName = "\\\\fs\\Group Projects\\UBS\\База конфигураций\\174 УБС\\commod.bin";

	cout << "Initialization of DriversIOManager, DriverManager, WorkManager, CConfiguration\n" << endl;


	auto drIoManager = static_cast<DriversIOManager *>(pMyFactory->CreateDriversIoManager());
	auto drManager = static_cast<DriverManager *>(pMyFactory->CreateDriverManager());
	drManager->setI2cCommodFileName(commodToValidateFileName);
	auto testWorkManager = static_cast<WorkManager *>(pMyFactory->CreateWorkManager(drIoManager, drManager));
	cout << "Configuration file validation: " << drManager->getI2cCommodFileName() << endl;
	auto validConfig = testWorkManager->validateConfig();

	//better to free library, be aware of memory leak. (automatically free library when there is no process using it)
	FreeLibrary(hInstDll);
	cout << "Library successfully freed." << endl;

	/*CHAR fileName[MAX_PATH];
	GetModuleFileName(nullptr, fileName, MAX_PATH);
	wstring wstr(fileName);
	string str(wstr.begin(), wstr.end());
	cout << str << endl;*/


	system("PAUSE");
	_CrtDumpMemoryLeaks();
	return 0;
}



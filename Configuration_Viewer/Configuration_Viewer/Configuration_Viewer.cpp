#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include <windows.h>
#include <iostream>
#include <string>

#define EMULATION
#define CLOCK_FILE_SIZE 64
#define EEPROMFILE_SIZE 128*1024

#include "ucu_fw/src/dllapi/factory.h"
#include "StrategyDeployment .h"

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
	//LOGIC
	//validate config
	//parse if needed
	//load
	//FORMAT
	//-commodFileName(destination of commod.bin) -a(parse) -z(zip only if parsed) -7(zip level) -s(save zipped file to the same location as unzipped)
	//example: -D:\commod.bin -a -7 
	setlocale(LC_ALL, "RUS");

	StrategyDeployment manager("C:\\Users\\Gulkevich_A\\Desktop\\current\\commod.bin");
	//manager.convert();
	manager.validateCurrentConfiguration();
	manager.showLog();
	system("PAUSE");
	_CrtDumpMemoryLeaks();
	return 0;
}



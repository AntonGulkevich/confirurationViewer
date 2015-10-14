#include "StrategyDeployment .h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

StrategyDeployment::StrategyDeployment(const std::string commodFileName_) :
	commodFileName(commodFileName_)
{
	zipEnabled = false;
	parseEnabled = false;
	saveToSameFolder = true;
	zipCompressionLevel = 7;
	createCompressedFiled = false;
	zipLocation = "D:\\Program Files (x86)\\7-Zip\\7z.exe";
	hInstDll = nullptr;

}

bool StrategyDeployment::zip(const std::string& sourceFileName, const std::string zippedFileName) 
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	auto appParam = "7z a -tzip -mx" + std::to_string(zipCompressionLevel) + "\"" + zippedFileName +"\"" + " \"" + sourceFileName + "\"";
	if (!CreateProcessA(zipLocation.c_str(),
		_strdup(appParam.c_str()),
		nullptr,           // Process handle not inheritable
		nullptr,           // Thread handle not inheritable
		FALSE,			   // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,  // Don't show 7zip log    
		nullptr,           // Use parent's environment block
		nullptr,           // Use parent's starting directory 
		&si,               // Pointer to STARTUPINFO structure
		&pi)               // Pointer to PROCESS_INFORMATION structure
		)
	{
		logList.push_back("CreateProcess failed: " + GetLastError());
		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	logList.push_back("File: " + sourceFileName + ". Succsessully compressed to " + sourceFileName);
	return true;
}

bool StrategyDeployment::unzip(const std::string zippedFileName) const
{
	//no need
	return true;
}

bool StrategyDeployment::openfile()
{
	return false;
	return true;
}

void StrategyDeployment::execute()
{
}

StrategyDeployment::~StrategyDeployment()
{
	if (commodFile != nullptr) delete commodFile;
	if (hInstDll != nullptr) FreeLibrary(hInstDll);
}
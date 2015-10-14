#include "StrategyDeployment .h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>

StrategyDeployment::StrategyDeployment(const std::string commodFileName_) :
	commodFileName(commodFileName_)
{
	zipEnabled = false;
	parseEnabled = false;
	saveToSameFolder = true;
	zipCompressionLevel = 7;
	createCompressedFiled = false;
	zipLocation = "D:\\Program Files (x86)\\7-Zip\\7z.exe";

}

bool StrategyDeployment::zip(const std::string& sourceFileName, const std::string zippedFileName)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	auto appParam = "7z a -tzip -mx7 \"" + zippedFileName +"\"" + " \"" + sourceFileName + "\"";	
	if (!CreateProcessA(zipLocation.c_str(),
		_strdup(appParam.c_str()),
		nullptr,           // Process handle not inheritable
		nullptr,           // Thread handle not inheritable
		FALSE,			   // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,      
		nullptr,           // Use parent's environment block
		nullptr,           // Use parent's starting directory 
		&si,               // Pointer to STARTUPINFO structure
		&pi)               // Pointer to PROCESS_INFORMATION structure
		)
	{
		std::cout<<"CreateProcess failed "<< GetLastError()<< std::endl;
		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

void StrategyDeployment::execute()
{
}

#include "StrategyDeployment .h"
#include <tchar.h>


StrategyDeployment::StrategyDeployment(const std::string commodFileName_) :
	commodFileName(commodFileName_)
{
	zipEnabled = false;
	parseEnabled = false;
	saveToSameFolder = true;
	zipCompressionLevel = 7;
	createCompressedFile = false;
	zipLocation = "D:\\Program Files (x86)\\7-Zip\\7z.exe";
	commodFile = nullptr;
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
		logList.push_back("CreateProcess failed. Error: " + GetLastError());
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

bool StrategyDeployment::saveFile(const std::string fileName, unsigned char *buffer, long size, const std::string &param)
{
	FILE *saveFile;
	if (fopen_s(&saveFile, fileName.c_str(), param.c_str())!=0)
	{
		logList.push_back("Unable to open file for write: " + fileName + ". Error: "+ std::to_string(GetLastError()));
		return false;
	}
	if (!fwrite(buffer, sizeof(char), size, saveFile))
	{
		logList.push_back("Unable to saveFile: " + fileName + ". Error: " + std::to_string(GetLastError()));
		return false;
	}
	fclose(saveFile);
	return true;
}

bool StrategyDeployment::saveFile(const std::string fileName,  std::vector<unsigned char> vecToSave)
{
	int size = vecToSave.size();
	unsigned char * buffer = new unsigned char[size];
	stdext::checked_array_iterator<unsigned char *> chkd_test_array(buffer, size);
	std::copy_n(vecToSave.begin(), size, chkd_test_array);
	bool processOk = saveFile(fileName, buffer, size, "w+b");
	delete[] buffer;
	return processOk;
}

bool StrategyDeployment::openfile(const std::string fileName)
{
	if (!isFileExists(fileName))
	{
		logList.push_back("File:" + fileName + " is not exist");
		return false;
	}
	if (fopen_s(&commodFile, fileName.c_str(), "r+b")!=0)
		logList.push_back("Error to open file:" + fileName + ". Error: " + std::to_string(GetLastError()));
	commodFileSize = getFileSize(fileName);
	return true;
}

bool StrategyDeployment::convert()
{
	//parsing file
	//parsing header
	auto header = new char[HEADER_SIZE];
	fread_s(header, HEADER_SIZE, sizeof(char), HEADER_SIZE, commodFile);

	//parsing data
	int dataSize = commodFileSize - HEADER_SIZE;
	auto dataToParse = new char[dataSize];
	fseek(commodFile, HEADER_SIZE, SEEK_SET);
	fread_s(dataToParse, dataSize, sizeof(char), dataSize, commodFile);
	std::list <std::string> stringList;
	std::string dataString(dataToParse);
	std::string delimiter = DELIMITER;
	size_t pos = 0;
	std::string token;
	while ((pos = dataString.find(delimiter)) != std::string::npos) 
	{
		token = dataString.substr(0, pos);
		stringList.push_back(token);
		dataString.erase(0, pos + delimiter.length());
	}
	dataString = "";
	stringList.push_back(token);
	stringList.pop_front();//the firts part is always zero
	auto address = HEADER_SIZE;
	std::list <Fat> headerList;
	for (auto it = stringList.begin(); it != stringList.end(); ++it)
	{
		auto temp = DELIMITER + *it;
		temp = temp.erase((temp.find("\r\nEND\r\n") + 7), temp.size());
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(=)[ ]{0,}"), std::string("="));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(==)[ ]{0,}"), std::string("=="));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(>)[ ]{0,}"), std::string(">"));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(<)[ ]{0,}"), std::string("<"));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(\\&\\&)[ ]{0,}"), std::string("&&"));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(\\|\\|)[ ]{0,}"), std::string("||"));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(\\})[ ]{0,}"), std::string("}"));
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(\\{)[ ]{0,}"), std::string("{"));
		temp = boost::regex_replace(temp, boost::regex("\t"), std::string(""));

		//deleting comments
		auto charsToDel = 0;
		auto start = 0;
		int tempCount = temp.size() - 1;
		for (auto j = 0; j<tempCount; ++j)
		{
			if (temp[j] == '/'&&temp[j + 1] == '/')
			{
				start = j;
				charsToDel = 0;
				while (temp[j++] != '\r')
					charsToDel++;

				temp.erase(start, charsToDel);
				j -= charsToDel;
				tempCount = tempCount - charsToDel;
			}
		}
		tempCount = temp.size() - 1;
		for (auto j = 0; j<tempCount; ++j)
		{
			if (temp[j] == ' '&&temp[j + 1] == ' ')
			{
				start = j;
				charsToDel = 0;
				while (temp[j++] == ' ')
					charsToDel++;

				temp.erase(start, charsToDel);
				j -= charsToDel;
				tempCount = tempCount - charsToDel;
			}
		}
		temp = boost::regex_replace(temp, boost::regex("(\r\n){2,}"), std::string("\r\n"));
		replaceAll(temp, "}\r\n", "}");
		replaceAll(temp, "{\r\n", "{");
		temp = boost::regex_replace(temp, boost::regex("[ ]{0,}(;\r\n)[ ]{0,}"), std::string(";"));
		/*text part must be multiply of 256: fill 0xFF*/
		tempCount = temp.size();
		int bytesToFill = HEADER_SIZE - tempCount%HEADER_SIZE;
		auto fillStr = new char [bytesToFill] ;
		memset(fillStr, 0xFF, bytesToFill);
		temp.append(fillStr, bytesToFill);

		/*calc header*/
		Fat tempFat;
		tempFat.address = address;
		tempFat.size = tempCount;
		address += temp.size();
		headerList.push_back(tempFat);
		/*end of cal header*/
		*it = temp;
		dataString += temp ;
		delete[] fillStr;
	}
	/*create new header for converted file*/
	std::vector<BYTE> headerNew;
	auto start = 0;
	for (auto it = headerList.begin(); it != headerList.end(); ++it)
	{
		headerNew.push_back(static_cast<BYTE>(((*it).address & (0xFF << 8)) >> 8));
		headerNew.push_back(static_cast<BYTE>(((*it).address & (0xFF << 16)) >> 16));

		headerNew.push_back(static_cast<BYTE>(((*it).size & (0xFF << 0)) >> 0));
		headerNew.push_back(static_cast<BYTE>(((*it).size & (0xFF << 8)) >> 8));
	}
	for (size_t headerVecotrSize = headerNew.size(); headerVecotrSize != HEADER_SIZE; ++headerVecotrSize)
		headerNew.push_back(0xFF);
	//done creating new header

	bool isOk = saveFile(commodFileName + "_a", headerNew);
	std::string resultStr;
	resultStr.append(dataString);
	isOk=isOk&saveFile(commodFileName + "_a", reinterpret_cast<unsigned char*>(_strdup(resultStr.c_str())), resultStr.size(), "a+b");

	isOk ? logList.push_back("File: " + commodFileName + " sucsessfully converted to\nFile: " + commodFileName + "_a.") :
		logList.push_back("Error converting file " + commodFileName);

	delete[] header;
	delete[] dataToParse;
	return isOk;
}

bool StrategyDeployment::validateCurrentConfiguration()
{
	auto hInstDll = LoadLibrary(_T("D:\\ubs\\Dev\\lib\\win32DLib\\x64\\Debug\\win32dlib.dll"));
	hInstDll!=nullptr ? logList.push_back("Dll loaded successfully.") : logList.push_back("Failed to load dll\n");
	auto pDllGetFactory = reinterpret_cast<DLLGETFACTORY>(GetProcAddress(hInstDll, "returnFactory"));
	auto pMyFactory = (pDllGetFactory)();
	if (pMyFactory == nullptr) return 0;
	auto drIoManager = static_cast<DriversIOManager *>(pMyFactory->CreateDriversIoManager());
	auto drManager = static_cast<DriverManager *>(pMyFactory->CreateDriverManager());
	drManager->setI2cCommodFileName(commodFileName);
	auto testWorkManager = static_cast<WorkManager *>(pMyFactory->CreateWorkManager(drIoManager, drManager));
	logList.push_back("Configuration file validation: " + drManager->getI2cCommodFileName());
	auto validConfig = testWorkManager->validateConfig();




	//better to free library, be aware of memory leak. (automatically free library when there is no process using it)
	FreeLibrary(hInstDll);
	logList.push_back("Library successfully freed.");
	return validConfig;
}

void StrategyDeployment::showLog()
{
	for (auto it = logList.begin(); it != logList.end(); ++it)
		std::cout << *it << std::endl;
}

StrategyDeployment::~StrategyDeployment()
{
	if (commodFile != nullptr) fclose(commodFile);
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

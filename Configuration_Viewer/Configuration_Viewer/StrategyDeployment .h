#ifndef STRATEGYDEPLOYMENT_H
#define STRATEGYDEPLOYMENT_H

#include <string>
#include <list>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <boost/regex.hpp>

#define HEADER_SIZE 256
#define CLOCK_FILE_SIZE 64
#define EEPROMFILE_SIZE 128*1024
#undef  EM_DEBUG

#define DELIMITER "GENERAL"
#include "SplitString.h"
#include "ftd2xx.h"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/crc.hpp"
#include "D:\ubs\Dev\lib\win32DLib\win32DLib\ucu_fw\src\dllapi\factory.h"
#include "crc32.h"

class StrategyDeployment 
{
private:
	std::string commodFileName;
	std::string folderSaveName;
	std::string zipLocation;
	bool parseEnabled;
	bool zipEnabled;
	bool saveToSameFolder;
	bool createCompressedFile;
	short zipCompressionLevel;//1-9

	FILE* commodFile;
	long commodFileSize;
	std::list<std::string> logList;

	struct Fat {
		unsigned int address;
		unsigned int size;
	};
	//ft_device
	std::list <FT_DEVICE_LIST_INFO_NODE> deviceList;

public:
	explicit StrategyDeployment(const std::string commodFileName_);
	//get
	std::string getCommodFileName()	{return commodFileName;}
	short getZipCompressionLevel() const { return zipCompressionLevel; }
	std::string getZipLocation() const  { return zipLocation; }
	bool isParseEnabked() const  { return parseEnabled; }
	bool isZipEnabled() const  { return zipEnabled; }
	bool isSaveToSameFolder() const  { return saveToSameFolder; }
	bool isCreateCompressedFile()const { return createCompressedFile; }
	
	//set
	void setZipLocation(const std::string & location) { zipLocation = location; }
	void setzipCompressionLevel(short level) { zipCompressionLevel = level; }
	void setParse(bool state) { parseEnabled = state;}
	void setZip(bool state) { zipEnabled = state; }
	void zetSaveToSameFolder(bool state) { saveToSameFolder = state; }
	void setCreateCompressedFile(bool state) { createCompressedFile = state; }

	//compression
	bool zip(const std::string &sourceFileName, const std::string zippedFileName);
	bool unzip(const std::string zippedFileName) const;
	bool to_zip();

	//operations with files
	bool saveFile(const std::string fileName, unsigned char *buffer, long size, const std::string &param);
	bool saveFile(const std::string fileName, const std::vector<unsigned char> vecToSave);
	bool saveFile(const std::string fileName, const std::list<std::string> listToSave);
	bool openfile(const std::string fileName);
	bool isFileExists(const std::string& name) const{
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}
	long getFileSize(std::string filename) const{
		struct stat stat_buf;
		int rc = stat(filename.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
	}
	bool convert();
	bool validateCurrentConfiguration();
	int getCRC32Commod();
	void addIntToVect(int var, std::vector<unsigned char> &vector);
	
	//ft_device
	bool loadConfiguration();
	unsigned int getDevicesCount();
	FT_HANDLE getFirstDeviceHandle();
	FT_HANDLE getDeviceByDescription(const std::string description);
	FT_STATUS sendPacket(FT_HANDLE ftHandle, std::vector<unsigned char> &buffer, DWORD bytesToSend, LPDWORD lpdwBytesWritten);
	void createPacket(std::vector <unsigned char> &buffer);	void closeFTDI(FT_HANDLE ftHandle);

	//log
	void saveLog();
	void showLog();
	//interface
	bool execute();
	~StrategyDeployment();	
};
void replaceAll(std::string& str, const std::string& from, const std::string& to);
typedef Factory * (__stdcall *DLLGETFACTORY)(void);
#endif STRATEGYDEPLOYMENT_H

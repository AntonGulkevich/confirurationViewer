#ifndef STRATEGYDEPLOYMENT_H
#define STRATEGYDEPLOYMENT_H

#include <string>
#include <list>
#include <windows.h>
#include <memory.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
//#include <regex>
#include <boost/regex.hpp>

#define HEADER_SIZE 256
#define CLOCK_FILE_SIZE 64
#define EEPROMFILE_SIZE 128*1024

#define DELIMITER "GENERAL"
#include "SplitString.h"
class StrategyDeployment 
{
private:
	std::string commodFileName;
	std::string folderSaveName;
	std::string zipLocation;
	bool parseEnabled;
	bool zipEnabled;
	bool saveToSameFolder;
	bool createCompressedFiled;
	short zipCompressionLevel;//1-9

	FILE* commodFile;
	long commodFileSize;

	HINSTANCE hInstDll;
	std::list<std::string> logList;

	struct Fat {
		unsigned int address;
		unsigned int size;
	};
public:
	explicit StrategyDeployment(const std::string commodFileName_);
	//get
	std::string getCommodFileName()	{return commodFileName;}
	short getZipCompressionLevel() const { return zipCompressionLevel; }
	std::string getZipLocation() const  { return zipLocation; }
	bool isParseEnabked() const  { return parseEnabled; }
	bool isZipEnabled() const  { return zipEnabled; }
	bool isSaveToSameFolder() const  { return saveToSameFolder; }
	bool isCreateCompressedFile()const { return createCompressedFiled; }
	
	//set
	void setZipLocation(const std::string & location) { zipLocation = location; }
	void setzipCompressionLevel(short level) { zipCompressionLevel = level; }
	void setParse(bool state) { parseEnabled = state;}
	void setZip(bool state) { zipEnabled = state; }
	void zetSaveToSameFolder(bool state) { saveToSameFolder = state; }
	void setCreateCompressedFile(bool state) { createCompressedFiled = state; }

	//compression
	bool zip(const std::string &sourceFileName, const std::string zippedFileName);
	bool unzip(const std::string zippedFileName) const;

	//operations with files
	bool saveFile(const std::string fileName, char *buffer, long size);
	bool openfile(const std::string fileName);
	bool isFileExists(const std::string& name) const
	{
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}
	long getFileSize(std::string filename) const
	{
		struct stat stat_buf;
		int rc = stat(filename.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
	}
	bool convert();
	//log
	bool saveLog();
	void showLog();

	~StrategyDeployment();	
};
void replaceAll(std::string& str, const std::string& from, const std::string& to);
#endif STRATEGYDEPLOYMENT_H

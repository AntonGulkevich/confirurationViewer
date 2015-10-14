#pragma once

#include <string>
#include <list>
#include <windows.h>
#include <memory.h>
#include <iostream>
#include <stdio.h>
#include <stdio.h>

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

	FILE *commodFile;
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
	bool saveFile(const std::string fileName);
	bool openfile(const std::string fileName);
	bool isFileExists(const std::string& name) const
	{
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}
	bool convert();
	//log
	bool saveLog();

	~StrategyDeployment();	
};



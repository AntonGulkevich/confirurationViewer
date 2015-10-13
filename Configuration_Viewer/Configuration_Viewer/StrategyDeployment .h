#pragma once

#include <string>
class StrategyDeployment 
{
private:
	std::string commodFileName;
	std::string folderSaveName;
	std::string zipLocation;
	bool parseEnabled;
	bool zipEnabled;
	bool saveToSameFolder;
	short zipCompressionLevel;
public:
	explicit StrategyDeployment(const std::string commodFileName_);
	//get
	std::string getCommodFileName()	{return commodFileName;}
	short getZipCompressionLevel() { return zipCompressionLevel; }
	std::string getZipLocation() { return zipLocation; }
	bool isParseEnabked() { return parseEnabled; }
	bool isZipEnabled() { return zipEnabled; }
	bool isSaveToSameFolder() { return saveToSameFolder; }
	

	//set
	void setZipLocation(const std::string & location) { zipLocation = location; }
	void setzipCompressionLevel(short level) { zipCompressionLevel = level; }
	void setParse(bool state) { parseEnabled = state;}
	void setZip(bool state) { zipEnabled = state; }
	void zetSaveToSameFolder(bool state) { saveToSameFolder = state; }


	~StrategyDeployment(){};
	void execute();
};



#include "StrategyDeployment .h"

StrategyDeployment::StrategyDeployment(const std::string commodFileName_) :
	commodFileName(commodFileName_)
{
	zipEnabled = false;
	parseEnabled = false;
	saveToSameFolder = true;
	zipCompressionLevel = 7;
	createCompressedFiled = false;
	zipLocation = "undefined";

}

bool StrategyDeployment::zip(const std::string& sourceFileName, const std::string zippedFileName)
{
		
}

void StrategyDeployment::execute()
{
}

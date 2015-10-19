#include "StrategyDeployment .h"

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

bool loadCommod(const std::string &fileName)
{
	StrategyDeployment *manager = new StrategyDeployment(fileName);
	manager->execute();
	manager->showLog();
	delete manager;
	return true;
}

int main(int argc, char* argv[])
{
	//LOGIC
	//validate config
	//parse if needed
	//load
	//FORMAT
	//-commodFileName(destination of commod.bin) -a(parse) -7(zip level if compression needed) 
	//example: -D:\commod.bin -a -7 
	setlocale(LC_ALL, "RUS");	
	loadCommod("C:\\Users\\Gulkevich_A\\Desktop\\current\\1\\commod.bin");
	system("PAUSE");
	_CrtDumpMemoryLeaks();
	return 0;
}



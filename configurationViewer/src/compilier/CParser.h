#ifndef CPARSER_H
#define CPARSER_H

#include <string>
#include <vector>
#include "../types.h"
#include "../dllapi/icparser.h"

// Базовый класс. Класс разбирающий конфигурацию УБС

#define _FAST_PARSER

#define WRITEERROR      manager.utils.GetTerminal().WriteError
#define WRITEWARNING    manager.utils.GetTerminal().WriteWarning
#define WRITEOK         manager.utils.GetTerminal().WriteOk

class Console;
class Commod;

class CParser: public ICParser
{
public:
	// Конструктор
	CParser(Console* console, Commod* commod);
	// Деструктор
	virtual ~CParser();
	// Очистка парсера от предыдущего разбора -- (постараться от нее избавиться)
	virtual void Clear() override;


	int LoadConfiguration() override;

	static const char* GetTypeStr(IOTYPES type);
	static const char* GetTypeStrRus(IOTYPES type);


	static const char* keyWords[];

protected:
	Console* _console;
	Commod* _commod;

	std::string& GetIdName(BYTE index)	{ return id[index]; }
	//std::string& GetStrName(BYTE index)	{ return str[index]; }

	virtual BYTE GetSymbolType(char ch) = 0;
	virtual short GetNextParserState(short state, TOKEN t) = 0;
	virtual short GetNextScannerState(short state, char ch) = 0;
	virtual TOKEN ReduceSym(short state) = 0;
	virtual int ReduceRule(short rule) = 0;
	virtual int DefineSeparator(char* str) = 0;

	int StartParsing();
    void WriteError(TOKEN t, const char* format, ...) const;
    void WriteCriticalWarning(TOKEN t, const char* format, ...) const;
    void WriteWarning(TOKEN t, const char* format, ...) const;

    TOKEN GetNextToken();
	short IndexOfList(const char* llist[]) const;
	short IndexOfId(char* str);
	void Push(char ch);
	
	int PopSeparator();
	int PopString();
	int PopDec();
	int PopFloat();  
	int PopId(TOKENCLASS &tc);
	TOKEN AddToken(TOKENCLASS tc, BYTE data);
	void PrintTokenFlow() const;

	std::vector<std::string> id;
	float numFloat;
	UINT numDec;
	
	std::vector<char*> str;

	char stack[1024];
	WORD stackPtr;

#ifdef _UCU_EMULATION
	WORD curStr;
	BYTE curSym;
#endif
	
	int Parse();
	int WriteResult(int res) const;
	const char* GetIdName(TOKEN t);

	std::vector<TOKEN> tokenStack;
	TOKEN params[10];  // параметры для передачи в функцию AddAction()
	char curCh;

	IOTYPES curIOType;		// Текущий тип входа выхода
	BYTE curIO;			// Текущий номер входа выхода
	int existWarning; // Если есть критические варнинги то 1
	BYTE device;  // 0 то разбор конфигурации
    BYTE breakCount;  // Контроль прерывания с клавиатуры
};

#endif /* CPARSER_H*/


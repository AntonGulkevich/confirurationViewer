#ifndef CPARSERDEVICE_H
#define CPARSERDEVICE_H
#include <vector>

#include "cparser.h"
#include "../application/cprocessor.h"

class CPattern;
class CConfiguration;

// ����� ����������� �������� ���������.
class CParserDevice : public CParser
{
public:
	CParserDevice(BYTE lDevice, CConfiguration* config, CPattern& lPattern, Console* console, Commod* commod);
	virtual ~CParserDevice()	{}
	void Clear() override;
private:
	struct EXPRESSION
	{
		float value;
		TYPESTACK type;
	};
    enum SECTION
    {
        sGeneral,
        sInputs,
        sOutputs,
    };

	static short scannerContrTable[44][27];
	int DefineSeparator(char* str) override;
	BYTE GetSymbolType(char ch) override;
	short GetNextScannerState(short state, char ch) override	{return scannerContrTable[state][GetSymbolType(ch)];}
	TOKEN ReduceSym(short state) override;

	
	/// Generated code
#include "CParserDevice_h.inc" 

	SYMBOLS GetIdKeyWord(int id);
	SYMBOLS GetTokenSymbol(TOKEN t);
#ifdef _FAST_PARSER
	short GetNextParserState(short state, TOKEN t) override	{ return contrTable[state][GetTokenSymbol(t)]; }
#else
	short GetNextParserState(short state, TOKEN t);
#endif
	int AddAction(RULES rule);
	CProcessor::ACTIONS BinOpsByRule(RULES rule, float &p1, float p2) const;
	void AddBinOpParams(RULES rule);  // ��������� 2 ��������� ��� ��������, �������� �� �� ���������
	int ReduceRule(short rule) override;
	short Reverse(UINT label) const;

	std::vector<EXPRESSION> exprStack;
	std::vector<float> arrayBuff;
    SECTION curSection;  // ������� ������
	CProcessor* curChannel;
	CConfiguration* _config;
	CPattern& curPattern;
	const char* ioSecStr;   // ������ (����� ��� ������)
	short curLabel;		// ������� ����� ��� ARINC
	short curWordNum;	// ������� ����� ����� ��� ARINC
	IOTYPES curRegType;		// ������� ��� ����� ������
	bool inParams;
	bool parsingNC;  // � ������ ������ ����������� �������������� ����
	bool parsingNCLabel;  // � ������ ������ ����������� �������������� ����� (�����)
};



#endif /* CPARSERDEVICE_H */

#ifndef ARINCWORD_OUT_H
#define ARINCWORD_OUT_H
#include "../interfaces/ioutputvalue.h"
#include "../utilities/m_math.h"
#include "arincouttable.h"


class ArincOut;

class ArincWordOut : public IOutputValue
{
public:
	static void SetArincTablePtr(ArincOutTable* arincTable) { _arincTable = arincTable; }

	ArincWordOut(ArincOut* channel, BYTE label);
	virtual ~ArincWordOut() {}
	// Получить значение метки
	BYTE GetLabel() const	{ return _label; }
	// Установка периода в мс
	void SetPeriod(UINT period)
	{
		_period = period;
		_period20 = ROUND(period / 20.0f);
		_period20 =	_period20 == 0 ? 1 : _period20;
	}

	void SetIsOnline(bool val)  { _isOnline = val; }
	void UpdateValue() override;

	bool SetTableMode();


private:
	static ArincOutTable* _arincTable;

	// метка слова
	BYTE _label;
	//DWORD foo;
	ArincOut* _channel;
	//DWORD foo1;
	UINT _period;
	UINT _period20;
	bool _isOnline;

	ArincOutTable::Item* _tableItem;
};



#endif /* ARINCWORD_OUT_H */

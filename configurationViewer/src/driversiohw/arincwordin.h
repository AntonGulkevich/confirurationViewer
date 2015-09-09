#ifndef ARINCWORD_IN_H
#define ARINCWORD_IN_H
#include "../interfaces/iinputvalue.h"
#include "arinctypes.h"


class ArincIn;

class ArincWordIn : public IInputValue
{
public:
	ArincWordIn(ArincIn* channel, BYTE label);
	virtual ~ArincWordIn() {}
	// �������� �������� �����
	BYTE GetLabel() const	{ return _label; }

	void UpdateValue() override;
	// ���� ��� �������� ������������ ��� ������
	bool IsDataParity() const;
	void SetPeriodRange(UINT min, UINT max) { _periodMin = min; _periodMax = max; }

private:
	// ����� �����
	BYTE _label;
	ArincIn* _channel;
	UINT _periodMin;
	UINT _periodMax;
};



#endif /* ARINCWORD_IN_H */

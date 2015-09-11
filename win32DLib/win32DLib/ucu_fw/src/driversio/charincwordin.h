/*
 * charincin.h
 *
 *  Created on: 03 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef CHARINCWORDIN_H_
#define CHARINCWORDIN_H_

#include "ichannelin.h"

class ArincWordIn;
class ChArincIn;

class ChArincWordIn : public IChannelIn
{
private:
	ArincWordIn* _channel;
	ChArincIn* _patternChannel; // ������ � �������� �������� ����������� �����
	DWORD _dataMask; // ����� ������� ������ ��� ���������� �����

public:
	ChArincWordIn(CPattern* const pattern, ChArincIn* patternChannel, UINT number);
	virtual ~ChArincWordIn() {}

	void AssignChannel(IInputValue* ch) override	{ _channel = reinterpret_cast<ArincWordIn*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;

	ChArincIn* GetChannel() const	{ return _patternChannel; }
	bool IsTimeouted() const;


};

#endif /* CHARINCWORDIN_H_ */

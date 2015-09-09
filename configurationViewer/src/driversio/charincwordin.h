/*
 * charincin.h
 *
 *  Created on: 03 июня 2015 г.
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
	ChArincIn* _patternChannel; // Канала в паттерне которому принадлежит метка
	DWORD _dataMask; // Маска обрезки данных при распаковке слова

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

/*
 * charincin.h
 *
 *  Created on: 03 июня 2015 г.
 *      Author: Youbrin_A
 */

#ifndef CHARINCWORDOUT_H_
#define CHARINCWORDOUT_H_

#include "ichannelout.h"

class ArincWordOut;
class ChArincOut;

class ChArincWordOut : public IChannelOut
{
private:
	ArincWordOut* _channel;
	ChArincOut* _patternChannel; // Канала в паттерне которому принадлежит метка
	UINT _period;

public:
	ChArincWordOut(CPattern* const pattern, ChArincOut* patternChannel, UINT number);
	virtual ~ChArincWordOut() {}

	void AssignChannel(IOutputValue* ch) override;

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;
	void SetId(UINT id);

	UINT GetPeriod() const
	{ return _period; }
	void SetTableMode() const;
	bool IsOnline();

	ChArincOut* GetChannel() const
	{ return _patternChannel; }
};

#endif /* CHARINCWORDOUT_H_ */

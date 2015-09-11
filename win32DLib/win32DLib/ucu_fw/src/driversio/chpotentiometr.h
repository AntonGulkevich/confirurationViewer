/*
 * chpotentiometr.h
 *
 *  Created on: 01 θώνÿ 2015 γ.
 *      Author: Youbrin_A
 */

#ifndef CHPOTENTIOMETR_H_
#define CHPOTENTIOMETR_H_


#include "channelcalibration.h"
#include "ichannelin.h"

class Potentiometr;

class ChPotentiometr : public IChannelIn
{
private:
	Potentiometr* _channel;

public:
	ChPotentiometr(CPattern* const pattern, UINT number);

	virtual ~ChPotentiometr() {}

	void AssignChannel(IInputValue* ch) override	{ _channel = reinterpret_cast<Potentiometr*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;
};

#endif /* CHPOTENTIOMETR_H_ */

/*
 * chdescreetein.h
 *
 *  Created on: 01 θώνÿ 2015 γ.
 *      Author: Youbrin_A
 */

#ifndef CHDESCREETEIN_H_
#define CHDESCREETEIN_H_


#include "ichannelin.h"

class DescreeteInput;

class ChDescreeteIn : public IChannelIn
{
private:
	DescreeteInput* _channel;

public:
	ChDescreeteIn(CPattern* const pattern, UINT number);

	virtual ~ChDescreeteIn() {}

	void AssignChannel(IInputValue* ch) override	{ _channel = reinterpret_cast<DescreeteInput*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;
};

#endif /* CHPOTENTIOMETR_H_ */

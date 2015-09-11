/*
 * chnptout.h
 *
 *  Created on: 01 θώνÿ 2015 γ.
 *      Author: Youbrin_A
 */

#ifndef CHNPTOUT_H_
#define CHNPTOUT_H_
#include "ichannelout.h"

class NPTOut;

class ChNPTOut : public IChannelOut
{
private:
	NPTOut* _channel;

public:
	ChNPTOut(CPattern* const pattern, UINT number);
	virtual ~ChNPTOut() {}

	void AssignChannel(IOutputValue* ch) override
	{ _channel = reinterpret_cast<NPTOut*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;
};

#endif /* CHNPTOUT_H_ */

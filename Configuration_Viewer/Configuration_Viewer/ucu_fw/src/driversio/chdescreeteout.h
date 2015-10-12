/*
 * chdescreeteout.h
 *
 *  Created on: 01 θώνÿ 2015 γ.
 *      Author: Youbrin_A
 */

#ifndef CHDESCREETOUT_H_
#define CHDESCREETOUT_H_
#include "ichannelout.h"

class DescreeteOutput;

class ChDescreeteOut : public IChannelOut
{
private:
	DescreeteOutput* _channel;

public:
	ChDescreeteOut(CPattern* const pattern, UINT number);
	virtual ~ChDescreeteOut() {}

	void AssignChannel(IOutputValue* ch) override
	{ _channel = reinterpret_cast<DescreeteOutput*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;
};

#endif /* CHDESCREETOUT_H_ */

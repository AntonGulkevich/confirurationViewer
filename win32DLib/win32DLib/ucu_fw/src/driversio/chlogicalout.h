/*
 * chlogicalout.h
 *
 *  Created on: 01 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef CHLOGICALOUT_H_
#define CHLOGICALOUT_H_
#include "../interfaces/iinputvalue.h"
#include "ichannelout.h"

class ChLogicalOut : public IChannelOut, public IInputValue // �������� ���������� ������ ��� LogicalIn
{
private:

public:
	ChLogicalOut(CPattern* const pattern, UINT number);
	virtual ~ChLogicalOut() {}

	void AssignChannel(IOutputValue* ch) override
	{ }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;

	void UpdateValue() override;
};

#endif /* CHLOGICALOUT_H_ */

/*
 * nptcontrol.h
 *
 *  Created on: 07 июля 2015 г.
 *      Author: Youbrin_A
 */

#ifndef NPTCONTROL_H_
#define NPTCONTROL_H_
#include "analogin.h"


class NptControl : public AnalogIn
{
public:
	NptControl(DWORD baseAddress, DWORD inputOffset) : AnalogIn(baseAddress, inputOffset) {}
	virtual ~NptControl() {}

	virtual void UpdateValue() override;

	bool IsRefShifted() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x103C) == 0xC; }
};

#endif /* NPTCONTROL_H_ */

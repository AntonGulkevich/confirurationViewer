/*
 * termometr.h
 *
 *  Created on: 21 ��� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef TERMOMETR_H_
#define TERMOMETR_H_
#include "../types.h"

#ifndef EMULATION


#include <xadcps.h>

class Termometr
{

private:
	DWORD _deviceId;
	XAdcPs XAdcInst;      /* XADC driver instance */

	bool _isAvaible;

public:
	Termometr(DWORD deviceId);
	virtual ~Termometr() {}

	float GetCurrentTemperature() { auto TempRawData = XAdcPs_GetAdcData(&XAdcInst, XADCPS_CH_TEMP); return XAdcPs_RawToTemperature(TempRawData); }
};
#else
class Termometr
{

private:
	DWORD _deviceId;
#ifndef EM_DEBUG 
	XAdcPs XAdcInst;      /* XADC driver instance */
#endif


	bool _isAvaible;

public:
	Termometr(DWORD deviceId);
	virtual ~Termometr() {}

	float GetCurrentTemperature() const;
};
#endif

#endif /* TERMOMETR_H_ */

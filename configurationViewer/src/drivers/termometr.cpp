/*
 * termometr.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: Youbrin_A
 */

#include "termometr.h"

#ifndef EMULATION 
	Termometr::Termometr(DWORD deviceId) : _deviceId(deviceId)
{
	auto ConfigPtr = XAdcPs_LookupConfig(_deviceId);
	if (ConfigPtr == NULL)
	{
		_isAvaible = false;
		return;
	}
	XAdcPs_CfgInitialize(&XAdcInst, ConfigPtr, ConfigPtr->BaseAddress);

	auto Status = XAdcPs_SelfTest(&XAdcInst);
	if (Status != XST_SUCCESS)
	{
		_isAvaible = false;
		return;
	}
	XAdcPs_SetSequencerMode(&XAdcInst, XADCPS_SEQ_MODE_SAFE);

}
#else
Termometr::Termometr(DWORD deviceId) : _deviceId(deviceId)
{
#ifndef EM_DEBUG 
	auto ConfigPtr = XAdcPs_LookupConfig(_deviceId);
	if (ConfigPtr == NULL)
	{
		_isAvaible = false;
		return;
	}
	XAdcPs_CfgInitialize(&XAdcInst, ConfigPtr, ConfigPtr->BaseAddress);

	auto Status = XAdcPs_SelfTest(&XAdcInst);
	if (Status != XST_SUCCESS)
	{
		_isAvaible = false;
		return;
	}
	XAdcPs_SetSequencerMode(&XAdcInst, XADCPS_SEQ_MODE_SAFE);
#endif


}
float Termometr::GetCurrentTemperature() const
{
#ifndef EM_DEBUG 
	auto TempRawData = XAdcPs_GetAdcData(&XAdcInst, XADCPS_CH_TEMP); 
	return XAdcPs_RawToTemperature(TempRawData);
#else
	float TempRawData = 10;
	return TempRawData;
#endif
}

#endif


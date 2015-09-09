/*
 * pldmemorycontrol.h
 *
 *  Created on: 12 рту. 2015 у.
 *      Author: Youbrin_A
 */

#ifndef PLDMEMORYCONTROL_H_
#define PLDMEMORYCONTROL_H_
#include "types.h"

class PldMemoryControl
{
private:
	static const int AddressCount = 70;

public:
	PldMemoryControl();
	virtual ~PldMemoryControl();

	static DWORD Addresses[AddressCount][2];

	DWORD UcuRead(UINT baseAddress, UINT offset) const;
	float UcuReadF(UINT baseAddress, UINT offset) const;
	void UcuWrite(UINT baseAddress, UINT offset, DWORD data) const;
	void UcuWriteF(UINT baseAddress, UINT offset, float data) const;

	bool TestAddress(DWORD address) const;

};

extern PldMemoryControl memControl;

#endif /* PLDMEMORYCONTROL_H_ */

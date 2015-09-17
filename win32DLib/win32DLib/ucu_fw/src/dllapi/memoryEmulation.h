#ifndef MEMORYEMULATION_H_
#define MEMORYEMULATION_H_
#include "../types.h"
#include "UbsLibrary.h"

#define UBS_MEM_SIZE 0xFFFFFFFF

class MemoryEmulation
{
private:
	static const int AddressCount = 70;
	BYTE * p_emulationMemory;

public:
	MemoryEmulation();
	virtual ~MemoryEmulation();

	static DWORD Addresses[AddressCount][2];

	DWORD UcuRead(UINT baseAddress, UINT offset) const;
	float UcuReadF(UINT baseAddress, UINT offset) const;
	void UcuWrite(UINT baseAddress, UINT offset, DWORD data) const;
	void UcuWriteF(UINT baseAddress, UINT offset, UCU_FLOAT data) const;

	bool TestAddress(DWORD address) const;

};

extern MemoryEmulation memControl;

#endif /* MEMORYEMULATION_H_ */

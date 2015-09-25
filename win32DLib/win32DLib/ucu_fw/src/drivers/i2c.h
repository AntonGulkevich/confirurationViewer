/*
 * i2c.h
 *
 *  Created on: 20 апр. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef I2C_H_
#define I2C_H_
#include "../types.h"

#ifndef EMULATION

#include "xiicps.h"


class I2C
{
private:
	XIicPs _iicInstance;
	UINT _iicDeviceId;
	UINT _slaveDeviceId;
	bool _isDeviceAvailable;
	UINT _addressSize;

	u8 _addressBuff[3];

	void FillAddress(UINT address);

public:

	I2C(UINT iicDdeviceId, UINT slaveDeviceId, UINT rate, UINT addressSize);
	virtual ~I2C() {}


	bool IsDeviceAvailable() { return _isDeviceAvailable; }

	void Read(void* buffer, DWORD address, UINT size);
	void Write(void* buffer, DWORD address, UINT size);
};
#else
#include <fstream>
#include <string>

class I2C
{
#ifndef EM_DEBUG 
	XIicPs _iicInstance;
#else
	std::fstream _iicInstance;
	std::string fileName;
#endif

	UINT _iicDeviceId;
	UINT _slaveDeviceId; 
	bool _isDeviceAvailable;
	UINT _addressSize;
	u8 _addressBuff[3];
	void FillAddress(UINT address);

public:
	I2C(UINT iicDdeviceId, UINT slaveDeviceId, UINT rate, UINT addressSize);
	virtual ~I2C() { _iicInstance.close(); }
	bool IsDeviceAvailable() const	{ return _isDeviceAvailable; }
	void Read(void* buffer, DWORD address, UINT size);
	void Write(void* buffer, DWORD address, UINT size);
};
#endif // EMULATION
#endif /* I2C_H_ */

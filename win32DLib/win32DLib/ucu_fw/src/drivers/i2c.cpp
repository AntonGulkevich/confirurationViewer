/*
 * i2c.cpp
 *
 *  Created on: 20 апр. 2015 г.
 *      Author: Youbrin_A
 */

#include "i2c.h"
#include "gpio.h"

#ifndef EMULATION

void status_handler(void* sender, u32 StatusEvent)
{
	if (StatusEvent == 0)
		for(;;);
}



I2C::I2C(UINT iicDdeviceId, UINT slaveDeviceId, UINT rate, UINT addressSize) : _iicDeviceId(iicDdeviceId), _slaveDeviceId(slaveDeviceId), _addressSize(addressSize)
{
	XIicPs_Config *ConfigPtr;	/* Pointer to configuration data */
	ConfigPtr = XIicPs_LookupConfig(_iicDeviceId);
	XIicPs_CfgInitialize(&_iicInstance, ConfigPtr, ConfigPtr->BaseAddress);
	if (XIicPs_SelfTest(&_iicInstance) == 0)
	{
		//XIicPs_SetStatusHandler(&_iicInstance, this, status_handler);
		XIicPs_SetSClk(&_iicInstance, rate);
		if (XIicPs_BusIsBusy(&_iicInstance))
			XIicPs_Abort(&_iicInstance);
		// Проверка наличия КМ
		FillAddress(0);
		_isDeviceAvailable = XIicPs_MasterSendPolled(&_iicInstance, _addressBuff, _addressSize, _slaveDeviceId) == 0;
	} else
		_isDeviceAvailable = false;
}

void I2C::FillAddress(UINT address)
{
	if (_addressSize == 1)
	{
		_addressBuff[0] = address & 0xFF;
		_addressBuff[2] = 0;
	}
	else if (_addressSize == 2)
	{
		_addressBuff[0] = (address >> 8) & 0xFF;
		_addressBuff[1] = address & 0xFF;
		_addressBuff[2] = ((address >> 16) & 1);
	}

}

void I2C::Read(void* buffer, DWORD address, UINT size)
{
	if (_isDeviceAvailable)
	{
		while (XIicPs_BusIsBusy(&_iicInstance));
		FillAddress(address);
		XIicPs_MasterSendPolled(&_iicInstance, _addressBuff, _addressSize, _slaveDeviceId | _addressBuff[2]);
		while (XIicPs_BusIsBusy(&_iicInstance));
		XIicPs_MasterRecvPolled(&_iicInstance, reinterpret_cast<u8*>(buffer), size, _slaveDeviceId  | _addressBuff[2]);
		while (XIicPs_BusIsBusy(&_iicInstance));
		Gpio::SVSet();
	}
}

void I2C::Write(void* buffer, DWORD address, UINT size)
{
	// Для записи в КМ - вызвать функцию снятия защиты от записи
	// Для записи в EEPROM - не забыть вставить джампер
	// Для записи в часы, проверить адреса 64б
	if (_isDeviceAvailable)
	{
		while (XIicPs_BusIsBusy(&_iicInstance)) {}
		u8 addr[size + _addressSize];
		FillAddress(address);
		memcpy(addr, _addressBuff, _addressSize);
		memcpy(&addr[_addressSize], buffer, size);
		XIicPs_MasterSendPolled(&_iicInstance, addr, sizeof(addr), _slaveDeviceId  | _addressBuff[2]);
		PerfomanceCounter::WaitMs(10); // В DataSheet Write Cycle Time
	}
}
#else

I2C::I2C(UINT iicDdeviceId, UINT slaveDeviceId, UINT rate, UINT addressSize) : _iicDeviceId(iicDdeviceId), _slaveDeviceId(slaveDeviceId), _addressSize(addressSize)
{
#ifndef EM_DEBUG 
	XIicPs_Config *ConfigPtr;	/* Pointer to configuration data */
	ConfigPtr = XIicPs_LookupConfig(_iicDeviceId);
	XIicPs_CfgInitialize(&_iicInstance, ConfigPtr, ConfigPtr->BaseAddress);
	if (XIicPs_SelfTest(&_iicInstance) == 0)
	{
		XIicPs_SetSClk(&_iicInstance, rate);
		if (XIicPs_BusIsBusy(&_iicInstance))
			XIicPs_Abort(&_iicInstance);
	// Проверка наличия КМ
		FillAddress(0);
		_isDeviceAvailable = XIicPs_MasterSendPolled(&_iicInstance, _addressBuff, _addressSize, _slaveDeviceId) == 0;
	}
	else
		_isDeviceAvailable = false;
#else
	enum
	{
		COMMOD,
		CLOCK,
		EEPROM
	};
	std::string fileName;
	switch (iicDdeviceId)
	{
	case COMMOD:
		fileName = "commod.bin";
		break;
	case CLOCK:
		fileName = "clock.bin";
		break;
	case EEPROM:
		fileName = "eeprom.bin";
		break;
	default:
		fileName = "def.bin";
	}
	_iicInstance.open(fileName, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	_iicInstance.is_open() ? _isDeviceAvailable = true : _isDeviceAvailable = false;
#endif

}

void I2C::FillAddress(UINT address)
{
	if (_addressSize == 1)
	{
		_addressBuff[0] = address & 0xFF;
		_addressBuff[2] = 0;
	}
	else if (_addressSize == 2)
	{
		_addressBuff[0] = (address >> 8) & 0xFF;
		_addressBuff[1] = address & 0xFF;
		_addressBuff[2] = ((address >> 16) & 1);
	}
}

void I2C::Read(void* buffer, DWORD address, UINT size) 
{
	if (_isDeviceAvailable)
	{
#ifndef EM_DEBUG 
		while (XIicPs_BusIsBusy(&_iicInstance));
		FillAddress(address);
		XIicPs_MasterSendPolled(&_iicInstance, _addressBuff, _addressSize, _slaveDeviceId | _addressBuff[2]);
		while (XIicPs_BusIsBusy(&_iicInstance));
		XIicPs_MasterRecvPolled(&_iicInstance, reinterpret_cast<u8*>(buffer), size, _slaveDeviceId | _addressBuff[2]);
		while (XIicPs_BusIsBusy(&_iicInstance));
#else
		_iicInstance.seekg(address, std::ios_base::beg);
		_iicInstance.read(static_cast<char *>(buffer), size);
#endif//emdebug		
		Gpio::SVSet();
	}
}

void I2C::Write(void* buffer, DWORD address, UINT size)
{
	if (_isDeviceAvailable)
	{
#ifndef EM_DEBUG 
	while (XIicPs_BusIsBusy(&_iicInstance)) {}
		u8 addr[size + _addressSize];
		FillAddress(address);
		memcpy(addr, _addressBuff, _addressSize);
		memcpy(&addr[_addressSize], buffer, size);
		XIicPs_MasterSendPolled(&_iicInstance, addr, sizeof(addr), _slaveDeviceId | _addressBuff[2]);
		PerfomanceCounter::WaitMs(10); // В DataSheet Write Cycle Time
#else
		_iicInstance.seekg(address, std::ios_base::beg);
		_iicInstance.write(static_cast<char *>(buffer), size);
#endif//emdebug
	}
}

#endif
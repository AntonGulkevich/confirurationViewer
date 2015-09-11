/*
 * drivermanager.h
 *
 *  Created on: 27 апр. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef DRIVERMANAGER_H_
#define DRIVERMANAGER_H_
#include "drivers/clock.h"
#include "drivers/display.h"
#include "drivers/i2c.h"
#include "drivers/irqmanager.h"
#include "drivers/keys.h"
#include "drivers/qspi.h"
#include "drivers/usbtr.h"
#include "drivers/gpio.h"
#include "drivers/perfomancecounter.h"
#include "drivers/termometr.h"
#include "flagmanager.h"
#include "interfaces/iusingirq.h"
#include "utilities/fwloader.h"
#include "utilities/commod.h"
#ifdef EMULATION 
#include  "../../ucu_fw_bsp/ps7_cortexa9_0/include/xparameters.h"
#endif 
#include "dllAPI/idrivermanager.h"

class DriverManager : IDriverManager, IUsingIrq
{
private:
	PerfomanceCounter _perfomanceCounter;
	Gpio _gpio;
	IrqManager _irqManager;
	FlagManager _flagManager;
	Display _display;
	UsbTr _usb;
	Qspi _flash;
	Keys _keys;
	FWLoader _fwLoader;
	I2C _i2cCommod;
	I2C _i2cRTC;
	I2C _i2cEEPROM;
	Clock _clock;
	Termometr _termometr;
	UINT _svSetValue;
public:

	DriverManager();
	virtual ~DriverManager() {}
	virtual void OnIrq(DWORD number) override;
	void EnableEthernet(bool isEnable) override	{ _gpio.SetOutput(0, isEnable ? 1 : 0); } // Enable Ethernet
	void EnableUsb(bool isEnable)override { _gpio.SetOutput(7, isEnable ? 1 : 0); } // Enable Ethernet
	void UsbFromPLD(bool fromPLD)override { _gpio.SetOutput(13, fromPLD ? 1 : 0); } // Enable Ethernet
	void EnableAllIrq()override { _irqManager.EnableAll(); }
	Gpio* GetGpio()override { return &_gpio; }
	FlagManager* GetFlagManager()override { return &_flagManager; }
	Display* GetDisplay() override { return &_display; }
	UsbTr* GetUSB() override { return &_usb; }
	Keys* GetKeys()override { return &_keys; }
	FWLoader* GetFWLoader()override { return &_fwLoader; }
	I2C* GetI2CCommod()override { return &_i2cCommod; }

	Clock* GetClock()override { return &_clock; }
	Termometr* GetTermometr() override { return &_termometr; }
	PerfomanceCounter* GetPerfomance()override { return &_perfomanceCounter; }


	void WriteEEPROM(void* buff, UINT address, UINT count)override;
	void ReadEEPROM(void* buff, UINT address, UINT count)override;
};

#endif /* DRIVERMANAGER_H_ */

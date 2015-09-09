/*
 * gpio.cpp
 *
 *  Created on: 20 апр. 2015 г.
 *      Author: Youbrin_A
 */

#include "gpio.h"
#ifndef EMULATION
XGpioPs Gpio::_gpio;
int Gpio::svSetValue = 1;

void Gpio::Init()
{

		XGpioPs_Config *ConfigPtr;

		ConfigPtr = XGpioPs_LookupConfig(_deviceId);
		XGpioPs_CfgInitialize(&_gpio, ConfigPtr,
						ConfigPtr->BaseAddr);
}


UINT Gpio::GetInput(UINT number)
{
	UINT data;
	XGpioPs_SetDirectionPin(&_gpio, number, 0);
	data = XGpioPs_ReadPin(&_gpio, number);
	return data;
}

void Gpio::SetOutput(UINT number, UINT value)
{
    XGpioPs_SetDirectionPin(&_gpio, number, 1);
    XGpioPs_SetOutputEnablePin(&_gpio, number, 1);
    XGpioPs_WritePin(&_gpio, number, value & 1);
    // Задержка?
}
#else

int Gpio::svSetValue = 1;

void Gpio::Init() const
{

}


UINT Gpio::GetInput(UINT number) const
{
	UINT data;
	// Тут эмуляция с интерфейса ПК
	return data;
}

void Gpio::SetOutput(UINT number, UINT value) const
{
	// Тут эмуляция на интерфейс ПК
}
#endif

 

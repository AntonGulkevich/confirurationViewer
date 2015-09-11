/*
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 *
 *
 * This file is a generated sample test application.
 *
 * This application is intended to test and/or illustrate some
 * functionality of your system.  The contents of this file may
 * vary depending on the IP in your system and may use existing
 * IP driver functions.  These drivers will be generated in your
 * SDK application project when you run the "Generate Libraries" menu item.
 *
 */

#include <stdio.h>
#include <cstdio>

#include "drivermanager.h"
#include "drivers/display.h"
#include "drivers/gpio.h"
#include "drivers/i2c.h"
#include "drivers/keys.h"
#include "types.h"
#include "driversiomanager.h"
#include "flagmanager.h"
#include "utilities/console.h"
#include "workmanager.h"

#ifndef EMULATION
#include <xil_cache.h>
#include "xparameters.h"
#else
#include "../../ucu_fw_bsp/ps7_cortexa9_0/include/xparameters.h"
#endif


/*
#include "lwip/udp.h"
#include "drivers/udp_drv.h"
#include "lwip/ip_addr.h"

struct udp_pcb* udp_sender1;
struct udp_pcb* udp_sender2;
struct udp_pcb* udp_receiver1;
struct udp_pcb* udp_receiver2;

#define inet_addr(a,b,c,d)    ( ((unsigned int )a) | ((unsigned int )b << 8) |\
                                ((unsigned int )c << 16) | ((unsigned int )d << 24) )

// ::: ��������� ��������� :::

// ������ ������� � �������������, ������������� �� 10 �� 1000.
#define TIMER_PERIOD 100
// IP-�����
#define IP_ADDRESS	(inet_addr(172,  22,   14, 100))
// ����� ����
#define NET_MASK 	(inet_addr(255,   0,    0,   0))
// ���� �� ���������
#define GATEWAY 	(inet_addr(172,  22,   22, 254))
// ������� (MAC) �����
unsigned char MAC_ADDRESS[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };


// ���������� �� ������ ������ �� UDP
void udp_handler1(void* data, unsigned int size)
{
	char vvv[1000];
	sprintf(vvv, "������� ������ �� UDP, ���� %d: %s",(int)12345 , data);
}

// ���������� �� ������ ������ �� UDP
void udp_handler2(void* data, unsigned int size)
{
	char vvv[1000];
	sprintf(vvv, "������� ������ �� UDP, ���� %d: %s",(int)12346 , data);

}

*/

int main() {

#ifndef EMULATION
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif

	DriverManager driverManager;
	driverManager.GetDisplay()->SetTextByText("3A  ");
	Gpio::SVSet();


	DriversIOManager driversIO;
	//driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_1, driversIO.GetADCControl()->IsInternalIrqEnabled());
	driverManager.GetDisplay()->SetTextByText("3A� ");

	Gpio::SVSet();
	WorkManager workManager(&driversIO, &driverManager);
	driverManager.GetDisplay()->SetTextByText("3A��");
	Gpio::SVSet();


	if (driverManager.GetClock()->IsHotStart())
	{
		Console::TraceLine("������� �����");

	driverManager.GetGpio()->SetLED(Gpio::LEDS::L7_1, true);

	}
	else
		Console::TraceLine("�������� �����");

	if (driverManager.GetI2CCommod()->IsDeviceAvailable() || driverManager.GetKeys()->IsBreakCombinationPressed())
	{

	workManager.Init();

	}
	else
	{
		driverManager.GetDisplay()->SetTextByText("A200");
		for(;;);
	}
	driverManager.EnableAllIrq();

	auto tickCount = driverManager.GetPerfomance()->GetValue();
	for(;;)
	{
/*		driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_1, driversIO.GetADCControl()->GetState().iostSync1Fail == 1);
		driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_2, driversIO.GetADCControl()->GetState().iostSync2Fail == 1);
		driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_3, driversIO.GetADCControl()->GetState().iostSync3Fail == 1);
		driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_4, driversIO.GetADCControl()->GetState().iostSync4Fail == 1);*/

		driverManager.GetFlagManager()->Process();
		if (driverManager.GetPerfomance()->GetValue()-tickCount > 40 * 50000)
		{
			tickCount = driverManager.GetPerfomance()->GetValue();
			if (!driversIO.IsIrqNormal())
				driverManager.GetDisplay()->SetTextByText("�000", true);
		}

	driverManager.GetGpio()->SetLED(Gpio::LEDS::L4_3, UCU_IORD_32DIRECT(XPAR_MAIN_ADC_HUB_0_BASEADDR, 0x1034) == 0);

	}

#ifndef EMULATION
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif
	return 0;
}

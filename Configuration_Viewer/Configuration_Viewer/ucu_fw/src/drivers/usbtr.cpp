/*
 * usbtr.cpp
 *
 *  Created on: 25 марта 2015 г.
 *      Author: Youbrin_A
 */



#include "usbtr.h"

#ifndef EMULATION

#include "../utilities/console.h"

DWORD UsbTr::_baseAddress = 0;

UsbTr::UsbTr(DWORD baseAddress, DWORD irqId) : _irqId(irqId)
{
	_baseAddress = baseAddress;
	_inputDiagAnalogPacketCount = 0;
	_inputDiagDigitalPacketCount = 0;
	_inputDiagAnalogPacketErrorCount = 0;
	_countOfXData1 = 0;
	_countOfXData2 = 0;
	_firstEditBuff = true;
	//SetRate(12000000);
}

UsbTr::~UsbTr()
{

}


void UsbTr::Init()
{
	RegIRQ(_irqId);
}

void UsbTr::OnIrq(DWORD number)
{
	auto status = GetStatus();
/*	if ((status & 0x2) == 0)
		Console::TraceLine("st = %02X, b = %d, m = %08X", status, UCU_IORD_32DIRECT(_baseAddress, 0x3C), GetMarker());*/
	UINT resetstatus = 0;
	if ((status & STATUS_MASK::EndDiagMarker) != 0)
	{
		if ((status & STATUS_MASK::DiagnosticAnalogReceived) != 0)
			_inputDiagAnalogPacketCount++;
		if ((status & STATUS_MASK::DiagnosticDigitalReceived) != 0)
			_inputDiagDigitalPacketCount++;
		resetstatus = status & (STATUS_MASK::DiagnosticAnalogReceived | STATUS_MASK::DiagnosticDigitalReceived | STATUS_MASK::EndDiagMarker);
	}
	if ((status & STATUS_MASK::SomeError) != 0)
	{
		if ((status & STATUS_MASK::DiagnosticAnalogReceived) != 0)
			_inputDiagAnalogPacketErrorCount++;
		/*if ((status & STATUS_MASK::DiagnosticDigitalReceived) != 0)
			_inputDiagDigitalPacketErrorCount++;*/
		resetstatus = status & (STATUS_MASK::DiagnosticAnalogReceived | STATUS_MASK::DiagnosticDigitalReceived);
	}
	if ((status & STATUS_MASK::DataXReceived) == STATUS_MASK::DataXReceived)
	{
		if ((status & STATUS_MASK::SomeError) == 0)
		{
			UINT count = GetPacketSize();
			/*if (count > 1024)
			{
				Console::TraceLine("count %d", count);
				return;
			}

			char buffer[65536];
			ReceiveData(buffer);
			for(UINT i = 0; i < count && i < 1024; i++)
				if (buffer[i] != (BYTE)i)
					break;*/
			if (*GetEditXDataSize() + count < XDataBufferSize && count > 8)
			{
				ReceiveData(GetEditXDataBuffer());
				DWORD marker = *reinterpret_cast<const DWORD*>(&GetEditXDataBuffer()[count-4]);
				if (marker == 0x3F344A21)
				{
					*GetEditXDataSize() += count-4;
					SetFlag(Flags::DataPCReceived);
				}
				else
					Console::TraceLine("Ошибочный пакет маркер конца %d, %08X", count, marker);
			}else
				ResetReadBuffer();
		}
		resetstatus |= STATUS_MASK::DataXReceived;
	}
	if(status & STATUS_MASK::TimeoutError)
	{
		Console::TraceLine("Timeout ожидалось %d, принято %d", GetPacketSize(), UCU_IORD_32DIRECT(_baseAddress, 0x3C));
	}
	ResetStatus(resetstatus | STATUS_MASK::SomeError);

}


void UsbTr::Receive64kData(void* data)
{
	BYTE* ldata = reinterpret_cast<BYTE*>(data);
	const UINT PACKET_SIZE = 64*1024;
	for(UINT i = 0; i < PACKET_SIZE; i++)
	{
		ldata[i] = UCU_IORD_32DIRECT(_baseAddress, dataOffset);
	}
}

UINT UsbTr::ReceiveData(void* data)
{
	UINT size = GetPacketSize();
	ResetReadBuffer();
	BYTE* ldata = reinterpret_cast<BYTE*>(data);
	for(UINT i = 0; i < size; i++)
	{
		ldata[i] = UCU_IORD_32DIRECT(_baseAddress, dataOffset);
	}
	return size;
}


void UsbTr::WriteData(const void* data, UINT size)
{
	while(IsWriteBufferBusy()) {}


	if (size == 0)
		SendAnswer(NoWriteData);
	else
	{
		UINT wordCount = (size + 3) >> 2;
		UCU_IOWR_32DIRECT(_baseAddress, 0x24, wordCount-1);
		const DWORD* ldata = reinterpret_cast<const DWORD*>(data);
		for(UINT i = 0; i < wordCount; i++)
		{
			UCU_IOWR_32DIRECT(_baseAddress, 0x1C, ldata[i]);
		}
		SendAnswer(WriteBufferReady);
	}

}

UsbTr::STATUS_MASK UsbTr::WaitForStatus(UINT mask)
{
	DWORD status;
	status = 0;
	UINT tactCount = 0;
	while((status & mask) == 0)
	{
		status = GetStatus();
		tactCount++;
		if ((tactCount & 0xFFFF) == 0)
			Gpio::SVSet();
	}
	return (STATUS_MASK)status;
}

UsbTr::STATUS_MASK UsbTr::WaitForStatusWithTry(UINT mask, int tryCount)
{
	DWORD status;
	status = 0;

	UINT tactCount = 0;
	while (tryCount > 0)
	{
		while((status & mask) == 0)
			status = GetStatus();
		if ((status & STATUS_MASK::SomeError) != 0)
		{
			tryCount--;
			ResetStatus(STATUS_MASK::SomeError);
			SendAnswer(PACKET_RESULT::ReTry);
		}
		else
			tryCount = 0;
		tactCount++;
		if ((tactCount & 0xFFF) == 0)
			Gpio::SVSet();
	}
	return (STATUS_MASK)status;
}

void UsbTr::SendProgress(UINT stage, UINT progressVal)
{
	StatusData sdata;
	sdata.marker = 0x51a1da1a;
	sdata.stage = stage;
	sdata.progress = progressVal;
	WriteData(&sdata, sizeof(StatusData));
}
#else
#include "../utilities/console.h"
#include <iostream>
#include <string>

DWORD UsbTr::_baseAddress = 0;

UsbTr::UsbTr(DWORD baseAddress, DWORD irqId) : _irqId(irqId)
{
	_baseAddress = baseAddress;
	_inputDiagAnalogPacketCount = 0;
	_inputDiagDigitalPacketCount = 0;

}

UsbTr::~UsbTr()
{

}

#define USB_DATA UCU_IORD_32DIRECT(_baseAddress, 0x4)

void UsbTr::Init()
{
	RegIRQ(_irqId);
}

void UsbTr::OnIrq(DWORD number)
{
	auto status = GetStatus();
	/*	if ((status & 0x2) == 0)
	Console::TraceLine("st = %02X, b = %d, m = %08X", status, UCU_IORD_32DIRECT(_baseAddress, 0x3C), GetMarker());*/
	UINT resetstatus = 0;
	if ((status & STATUS_MASK::EndDiagMarker) != 0)
	{
		if ((status & STATUS_MASK::DiagnosticAnalogReceived) != 0)
			_inputDiagAnalogPacketCount++;
		if ((status & STATUS_MASK::DiagnosticDigitalReceived) != 0)
			_inputDiagDigitalPacketCount++;
		resetstatus = status & (STATUS_MASK::DiagnosticAnalogReceived | STATUS_MASK::DiagnosticDigitalReceived | STATUS_MASK::EndDiagMarker);
	}
	if ((status & STATUS_MASK::SomeError) != 0)
	{
		if ((status & STATUS_MASK::DiagnosticAnalogReceived) != 0)
			_inputDiagAnalogPacketErrorCount++;
		/*if ((status & STATUS_MASK::DiagnosticDigitalReceived) != 0)
		_inputDiagDigitalPacketErrorCount++;*/
		resetstatus = status & (STATUS_MASK::DiagnosticAnalogReceived | STATUS_MASK::DiagnosticDigitalReceived);
	}
	if ((status & STATUS_MASK::DataXReceived) == STATUS_MASK::DataXReceived)
	{
		if ((status & STATUS_MASK::SomeError) == 0)
		{
			UINT count = GetPacketSize();
			/*if (count > 1024)
			{
			Console::TraceLine("count %d", count);
			return;
			}

			char buffer[65536];
			ReceiveData(buffer);
			for(UINT i = 0; i < count && i < 1024; i++)
			if (buffer[i] != (BYTE)i)
			break;*/
			if (*GetEditXDataSize() + count < XDataBufferSize && count > 8)
			{
				ReceiveData(GetEditXDataBuffer());
				DWORD marker = *reinterpret_cast<const DWORD*>(&GetEditXDataBuffer()[count - 4]);
				if (marker == 0x3F344A21)
				{
					*GetEditXDataSize() += count - 4;
					SetFlag(Flags::DataPCReceived);
				}
				else
					Console::TraceLine("Ошибочный пакет маркер конца %d, %08X", count, marker);
			}
			else
				ResetReadBuffer();
		}
		resetstatus |= STATUS_MASK::DataXReceived;
	}
	if (status & STATUS_MASK::TimeoutError)
	{
		Console::TraceLine("Timeout ожидалось %d, принято %d", GetPacketSize(), UCU_IORD_32DIRECT(_baseAddress, 0x3C));
	}
	ResetStatus(resetstatus | STATUS_MASK::SomeError);

}


void UsbTr::Receive64kData(void* data)
{
	BYTE* ldata = reinterpret_cast<BYTE*>(data);
	const UINT PACKET_SIZE = 64 * 1024;
	for (UINT i = 0; i < PACKET_SIZE; i++)
	{
		ldata[i] = UCU_IORD_32DIRECT(_baseAddress, dataOffset);
	}
}

UINT UsbTr::ReceiveData(void* data)
{
	UINT size = GetPacketSize();
	ResetReadBuffer();
	BYTE* ldata = reinterpret_cast<BYTE*>(data);
	for (UINT i = 0; i < size; i++)
	{
		ldata[i] = UCU_IORD_32DIRECT(_baseAddress, dataOffset);
	}
	return size;
}

void UsbTr::WriteData(const void* data, UINT size)
{
	/*while (IsWriteBufferBusy()) {}

	if (size == 0)
		SendAnswer(NoWriteData);
	else
	{
		auto wordCount = (size + 3) >> 2;
		UCU_IOWR_32DIRECT(_baseAddress, 0x24, wordCount - 1);
		const DWORD* ldata = reinterpret_cast<const DWORD*>(data);
		for (UINT i = 0; i < wordCount; i++)
		{
			UCU_IOWR_32DIRECT(_baseAddress, 0x1C, ldata[i]);
		}
		SendAnswer(WriteBufferReady);
	}*/
	setlocale(LC_ALL, "RUS");
	if (size < 8) return;
	char* buff = new char[size-7];
	memcpy(buff, &static_cast<const char*>(data)[8], size-8);
	std::cout << buff;
	delete[] buff;
}

UsbTr::STATUS_MASK UsbTr::WaitForStatus(UINT mask)
{
	DWORD status;
	status = 0;
	UINT tactCount = 0;
	while ((status & mask) == 0)
	{
		status = GetStatus();
		tactCount++;
		if ((tactCount & 0xFFFF) == 0)
			Gpio::SVSet();
	}
	return static_cast<STATUS_MASK>(status);
}

UsbTr::STATUS_MASK UsbTr::WaitForStatusWithTry(UINT mask, int tryCount)
{
	DWORD status;
	status = 0;

	UINT tactCount = 0;
	while (tryCount > 0)
	{
		while ((status & mask) == 0)
			status = GetStatus();
		if ((status & STATUS_MASK::SomeError) != 0)
		{
			tryCount--;
			ResetStatus(STATUS_MASK::SomeError);
			SendAnswer(PACKET_RESULT::ReTry);
		}
		else
			tryCount = 0;
		tactCount++;
		if ((tactCount & 0xFFF) == 0)
			Gpio::SVSet();
	}
	return static_cast<STATUS_MASK>(status);
}

void UsbTr::SendProgress(UINT stage, UINT progressVal)
{
	StatusData sdata;
	sdata.marker = 0x51a1da1a;
	sdata.stage = stage;
	sdata.progress = progressVal;
	WriteData(&sdata, sizeof(StatusData));
}

#endif

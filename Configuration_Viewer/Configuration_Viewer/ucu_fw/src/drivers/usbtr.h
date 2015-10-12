/*
 * usbtr.h
 *
 *  Created on: 25 ����� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef USBTR_H_
#define USBTR_H_
#include "../types.h"
#include "../interfaces/iusingflags.h"

#include "../interfaces/iusingirq.h"
#include "../drivers/gpio.h"

class UsbTr : public IUsingIrq, public IUsingFlags
{
public:

	enum PACKET_RESULT
	{
		NoWriteData = 0x22,
		WriteBufferReady = 0x11,
		Break = 0x00,
		Correct = 0x77,
		WrongCrc = 0x88,
		WrongPacketNumber = 0x99,
		ReTry = 0xAA,
	};

	enum STATUS_MASK
	{
		Data64Received = 1,
		DataXReceived = 2,
		DataSendRequest = 4,
		DiagnosticAnalogReceived = 8,
		DiagnosticDigitalReceived = 16,
		EndDiagMarker = 32,
		TimeoutError = 64,
		IncorrectMarker = 128,
		SomeError = TimeoutError + IncorrectMarker,
		All = 0xFFFFFFFF

	};

	// �������������
	struct StatusData
	{
		UINT marker;
		UINT stage;
		UINT progress;
	};


private:
	const UINT dataOffset = 0x4;

	static DWORD _baseAddress;
	DWORD _irqId;
	DWORD _inputDiagAnalogPacketCount;
	DWORD _inputDiagDigitalPacketCount;
	DWORD _inputDiagAnalogPacketErrorCount;

	DWORD GetStatus() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0xC); }

	bool IsWriteBufferBusy() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x38) != 0; }
	void ResetReadBuffer() const	{ UCU_IOWR_32DIRECT(_baseAddress, 0x34, 1); } // ������������� ������ � ������ ���� �� 0 �������
	// ������ ���������� ������
	UINT GetPacketSize() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x28) + 1; }
	UINT ReceiveData(void* data);

	static const UINT XDataBufferSize = 65536;

	BYTE _dataXBuffer1[XDataBufferSize];
	UINT _countOfXData1;
	BYTE _dataXBuffer2[XDataBufferSize];
	UINT _countOfXData2;
	bool _firstEditBuff;

	UINT* GetEditXDataSize() { return _firstEditBuff ?  &_countOfXData1 : &_countOfXData2; }
	BYTE* GetEditXDataBuffer() { return _firstEditBuff ? _dataXBuffer1 + _countOfXData1 : _dataXBuffer2 + _countOfXData2; }


public:

	void SetRate(UINT rate) { UCU_IOWR_32DIRECT(_baseAddress, 0x44, (UINT)(50000000.0 / rate + 0.5)-1); }
	UINT GetXDataSize() const { return _firstEditBuff ?  _countOfXData2 : _countOfXData1; }
	const BYTE* GetXDataBuffer() const { return  _firstEditBuff ? _dataXBuffer2 : _dataXBuffer1; }
	void StartXDataParse() { _firstEditBuff = !_firstEditBuff; *GetEditXDataSize() = 0; }

	void SetTimeout(UINT timeout) const	{ UCU_IOWR_32DIRECT(_baseAddress, 0x40, timeout * 50000);  }

	UsbTr(DWORD baseAddress, DWORD irqId);
	virtual ~UsbTr();

	void Init();

	virtual void OnIrq(DWORD number) override;

	// �������� ���������� �� USB
	STATUS_MASK WaitForStatus(UINT mask);
	// �������� ���������� �� USB ��������� ������� ��� ������
	STATUS_MASK WaitForStatusWithTry(UINT mask, int tryCount);

	void ResetStatus(UINT mask) const	{ UCU_IOWR_32DIRECT(_baseAddress, 0x20, mask); }
	// ��������� ���������� �� �����
	void MaskIrq(UINT mask) const	{ UCU_IOWR_32DIRECT(_baseAddress, 0x2C, mask); }

	void Receive64kData(void* data);
	// ������ ������ � �� �� �������. ���� size = 0, ������������ ����� ����� �� ������ ������
	void WriteData(const void* data, UINT size);

	UINT GetTotalPacketCount() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x8); }
	UINT GetCurrentPacketNumber() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x10); }
	UINT GetCRC() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x14); }
	UINT GetMarker() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x0); }
	UINT GetPLDDate() const	{ return UCU_IORD_32DIRECT(_baseAddress, 0x60); }


	// ����� ���� ������� ���������� ���� � ����� �� ������ �� ��
	void SendAnswer(PACKET_RESULT result) const	{ UCU_IOWR_32DIRECT(_baseAddress, 0x18, result); }
	void SendProgress(UINT stage, UINT progressVal);

	DWORD GetInputDiagAnalogPacketCount() { return _inputDiagAnalogPacketCount; }
	DWORD GetInputDiagDigitalPacketCount() { return _inputDiagDigitalPacketCount; }
	DWORD GetInputDiagAnalogPacketErrorCount() { return _inputDiagAnalogPacketErrorCount; }

};

#endif /* USBTR_H_ */

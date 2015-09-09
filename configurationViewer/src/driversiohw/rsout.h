/*
 * rsin.h
 *
 *  Created on: 19 ��� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef RSOUT_H_
#define RSOUT_H_
#include "../interfaces/ioutputvalue.h"
#include "../types.h"
#include "rs.h"

class RSOut : public IOutputValue
{
public:
	enum class WorkMode
	{
		Fifo,
		Packet
	};
	union Status
	{
		struct
		{
			UINT fifoempty : 1;
			UINT fifofull : 1;
			UINT : 30;

		};
		DWORD dword;
	};
	union WorkControl
	{
		struct
		{
			UINT workMode : 1;
			UINT isWork : 1;
			UINT irqEnable : 1;
			UINT updateBuffer : 1;
			UINT clearBuffer : 1;
			UINT : 27;

		};
		DWORD dword;
	};
private:
	const UINT BAUDRATE_OFFSET = 0x34;
	const UINT NUMBERBIT_OFFSET = 0x38;
	const UINT PARITY_OFFSET = 0x3C;
	const UINT STOPBITS_OFFSET = 0x40;
	const UINT FIFO_OFFSET = 0x4C;
	const UINT DATA_OFFSET = 0x50;
	const UINT STATUS_OFFSET = 0x54;
	const UINT WORKCONTROL_OFFSET = 0x44;

	const UINT TIME_OFFSET = 0x48;

	const UINT IRQENABLE_OFFSET = 0x54;

	DWORD _baseAddress;
	BYTE _outputBuffer[1024];
	UINT _sendCount;
	UINT _diagSendCount;

	WorkControl _workMode;

	Status GetStatus() { Status st; st.dword = UCU_IORD_32DIRECT(_baseAddress, STATUS_OFFSET); return st; }
	void ResetStatus() const	{}
	void ClearBuffer() { _workMode.clearBuffer = 1; UCU_IOWR_32DIRECT(_baseAddress, WORKCONTROL_OFFSET, _workMode.dword); _workMode.clearBuffer = 0; }
	void UpdateBuffer() { _workMode.updateBuffer = 1; UCU_IOWR_32DIRECT(_baseAddress, WORKCONTROL_OFFSET, _workMode.dword); _workMode.updateBuffer = 0; }


public:
	explicit RSOut(DWORD baseAddress);
	virtual ~RSOut() {}


	void SetBaudRate(RS::BaudRate rate) { UCU_IOWR_32DIRECT(_baseAddress, BAUDRATE_OFFSET, 50000000 / static_cast<UINT>(rate)); }
	void SetNumberBits(UINT val) { UCU_IOWR_32DIRECT(_baseAddress, NUMBERBIT_OFFSET, val); }
	void SetParityType(RS::ParityType parity) { UCU_IOWR_32DIRECT(_baseAddress, PARITY_OFFSET, static_cast<UINT>(parity)); }
	// �������� ���� 1 ��� 2
	void SetStopBits(UINT val) { UCU_IOWR_32DIRECT(_baseAddress, STOPBITS_OFFSET, val-1); }

	RS::BaudRate GetBaudRate() const	{ return static_cast<RS::BaudRate>(50000000 / UCU_IORD_32DIRECT(_baseAddress, BAUDRATE_OFFSET)); }
	UINT GetNumberBits() const	{ return UCU_IORD_32DIRECT(_baseAddress, NUMBERBIT_OFFSET); }
	RS::ParityType GetParityType() const	{ return static_cast<RS::ParityType>(UCU_IORD_32DIRECT(_baseAddress, PARITY_OFFSET)); }
	// �������� ���� 1 ��� 2
	UINT GetStopBits() const	{ return UCU_IORD_32DIRECT(_baseAddress, STOPBITS_OFFSET) + 1; }

	void SetWorkMode(WorkMode mode) { _workMode.workMode = static_cast<UINT>(mode); UCU_IOWR_32DIRECT(_baseAddress, WORKCONTROL_OFFSET, _workMode.dword); }
	// ���������� ������ ������
	void SetPeriod(DWORD time) { UCU_IOWR_32DIRECT(_baseAddress, TIME_OFFSET, time); }

	void SetBuffer(BYTE* buff, UINT count);
	void UpdateValue() override;

	// �����������
	UINT GetBufferSize() const	{ return _diagSendCount; }
	BYTE* GetBuffer() { return _outputBuffer; }
	void ResetDiagBuffer() { if (_workMode.workMode == static_cast<UINT>(WorkMode::Fifo)) _diagSendCount = 0; }
};

#endif /* RSOUT_H_ */

/*
 * ichannel.h
 *
 *  Created on: 01 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef ICHANNEL_H_
#define ICHANNEL_H_
#include "../types.h"
#include "channelcalibration.h"
#include "channelfilter.h"
#include "channelrate.h"
#include "cregister.h"
#include "../dllAPI/IIChannel.h"

class CPattern;

class IChannel : public IIChannel
{
public:
	explicit IChannel(CPattern* const pattern);
	virtual ~IChannel() {}
	UINT GetNumber() const override	{ return _number; }
	IOTYPES GetType() const override	{ return _ioType; }
	// ����� ��� �������� ���������� � ������� ����������
	UINT GetPatternIndex() const override; 
	// ����� ��� �����������
	UINT GetPatternDeviceNum() const override; 
	// ���������� ��������� �� ������� �� ID
	CRegister* GetRegister(REGISTER_ID id) override { return (id != REGISTER_ID::nullptrID) ? registers_t[static_cast<UINT>(id)].reg : nullptr; }
	// ���������� ��������� �� ��������� ��������� �� ID
	const REGISTER_TYPE* GetRegisterT(REGISTER_ID id) override  { return (id != REGISTER_ID::nullptrID) ? &registers_t[static_cast<UINT>(id)] : nullptr; }

	// �������� ������������ ������������
	virtual CheckConfigurationResult CheckConfiguration() override;

	virtual void InitRegisters()  = 0;
	virtual void UpdateDataToHW()  = 0;
	virtual void UpdateHWToData()  = 0;

	bool IsNumberType(IOTYPES type, UINT number) const override	{ return _ioType == type && _number == number; }
	bool IsConfigured() const	override { return _isConfigured; }
	bool IsHasCalibration() const	override { return _calibration != nullptr; }
	bool IsValueFiltered() const override { return _filter != nullptr; }
	bool IsRateCalculating() const override	{ return _rate != nullptr; }
	ChannelCalibration* GetChCalibration() const override { return _calibration; }
	ChannelFilter* GetChFilter() const override	{ return _filter; }
	ChannelRate* GetChRate() const override	{ return _rate; }

	// ���������� ��� ����������� �������
	void ResetCheckAlarm() override { _userAlarmCode = 0xFFFFFFFF; _isSupressAlarm = false; }
	void SetCheckAlarmCode(UINT code) override;
	UINT GetCheckAlarmCode() const override	{ return _userAlarmCode; }
	void SetSuppressAlarm(bool isSupress) override { _isSupressAlarm = isSupress; }
	bool GetSuppressAlarm() const override	{ return _isSupressAlarm; }
	ChannelData GetChannelIdentificator() override;
protected:
	REGISTER_TYPE registers_t[static_cast<UINT>(REGISTER_ID::COUNTREGISTERS)];
	IOTYPES _ioType;
	UINT _number;
	CPattern* const _pattern;
	// ������������ ��������� � ������ � ������
	bool _isConfigured; 
	// ����� ����� ������������
	ChannelCalibration* _calibration;
	// ����� ����� ����������� ��������
	ChannelFilter* _filter; 
	// ����� ����� ������� ��������
	ChannelRate* _rate; 

	void CreateRegisters();
	friend ChannelCalibration;
	friend ChannelFilter;
	friend ChannelRate;

	// ���������� ��� ����������� �������
	UINT _userAlarmCode;
	bool _isSupressAlarm;

};


#endif /* ICHANNEL_H_ */

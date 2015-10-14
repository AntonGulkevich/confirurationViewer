/*
 * chanalogin.cpp
 *
 *  Created on: 01 ���� 2015 �.
 *      Author: Youbrin_A
 */

#include "chanalogin.h"

#include "../driversiohw/analogin.h"
#include "../driversio/cregister.h"
#include "channelfilter.h"
#include "channelrate.h"

ChAnalogIn::ChAnalogIn(CPattern* const pattern, UINT number) : IChannelIn(pattern)
{
	_channel = nullptr;
	_ioType = IOTYPES::ioAnalog;
	_number = number;
	_calibration = new ChannelCalibration(this, true);
	_filter = new ChannelFilter(this);
	_rate = new ChannelRate(this);

	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
		registers_t[i].id = REGISTER_ID::NULLID;

	registers_t[static_cast<UINT>(REGISTER_ID::rNAME)] = {REGISTER_ID::rNAME, rwConstant, rtString, 0.0f, 0.0f, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rVALUE)] = {REGISTER_ID::rVALUE, rwVariable, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rSTATE)] = {REGISTER_ID::rSTATE, rwVariable, rtDec, 0.0f, 0xFFFF, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rTYPE)] = {REGISTER_ID::rTYPE, rwConstant, rtDec, 0.0f, 5.0f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rDELAY)] = {REGISTER_ID::rDELAY, rwUser, rtDec, 0.0f, 2499.9998f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rSYNCNUM)] = {REGISTER_ID::rSYNCNUM, rwConstant, rtDec, 0.0f, 4, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rDELTA)] = {REGISTER_ID::rDELTA, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rMAXLEVEL)] = {REGISTER_ID::rMAXLEVEL, rwConstant, rtDec, -100.0f, 100, 100.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rMINLEVEL)] = {REGISTER_ID::rMINLEVEL, rwConstant, rtDec, -100.0f, 100, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rGROUP)] = {REGISTER_ID::rGROUP, rwConstant, rtDec, 0.0f, 99.0f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rSUBGROUP)] = {REGISTER_ID::rSUBGROUP, rwConstant, rtDec, 1.0f, 9.0f, 1.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rUNIT)] = {REGISTER_ID::rUNIT, rwConstant, rtDec, 0.0f, 17.0f, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rDEADZONE)] = {REGISTER_ID::rDEADZONE, rwConstant, rtFloat, -360, 0, 0, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONDELTA)] = {REGISTER_ID::rCORRECTIONDELTA, rwUser, rtDec, 0, 360, 0, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)] = {REGISTER_ID::rCORRECTIONSIGN, rwUser, rtDec, -1, 1, 1, false};
	_calibration->CreateClibrationRegisters();
	_filter->CreateFilterRegisters();
	_rate->CreateRateRegisters();
	CreateRegisters();

}

ChAnalogIn::~ChAnalogIn()
{
	delete _calibration;
}

void ChAnalogIn::InitRegisters()
{
	_channel->SetSyncNumber(registers_t[static_cast<UINT>(REGISTER_ID::rSYNCNUM)].reg->GetValueUInt());
	_channel->SetDelay(registers_t[static_cast<UINT>(REGISTER_ID::rDELAY)].reg->GetValueUInt());
	// ����� �� ��������� �����
	if (registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->GetValueInt() != -1 &&
		registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->GetValueInt() != 1)
	{
		auto isFilled = registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->IsFilled();
		registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->SetValue(1);
		registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->SetFilled(isFilled);
	}
	// ������ minlevel ��� �������, ����� ��������� ������ ������� �������
	auto minLevel = registers_t[static_cast<UINT>(REGISTER_ID::rMINLEVEL)].reg->GetValueInt();
	auto maxLevel = registers_t[static_cast<UINT>(REGISTER_ID::rMAXLEVEL)].reg->GetValueInt();
	switch((ANALOG_TYPE)registers_t[static_cast<UINT>(REGISTER_ID::rTYPE)].reg->GetValueUInt())
	{
	case ANALOG_TYPE::SCT_8: // ��� 8�
		_channel->SetAngleType(AnalogIn::ANGLE_TYPE::SCT8);
		break;
	case ANALOG_TYPE::SCT_20: // ��� 20�
		_channel->SetAngleType(AnalogIn::ANGLE_TYPE::SCT20);
		break;
	case ANALOG_TYPE::SCT_36: // ��� 36�
		_channel->SetAngleType(AnalogIn::ANGLE_TYPE::SCT36);
		break;
	case ANALOG_TYPE::Selsin: // ������� 36�-60�
		_channel->SetAngleType(AnalogIn::ANGLE_TYPE::Selsin);
		break;
	case ANALOG_TYPE::Logometr: // �������� 27�
		_channel->SetAngleType(AnalogIn::ANGLE_TYPE::Logometr);
		break;
	case ANALOG_TYPE::NPT: // ���
	{
		float level;
		if (m_abs(minLevel) > m_abs(maxLevel))
			level = m_abs(minLevel);
		else
			level = m_abs(maxLevel);
		if (level > 50)
			_channel->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x01);
		if (level > 20)
			_channel->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x02);
		if (level > 10)
			_channel->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x05);
		_channel->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x1);
		_channel->SetType(AnalogIn::TYPE::NPT);
	}
		break;
	}
	if (registers_t[static_cast<UINT>(REGISTER_ID::rMINLEVEL)].reg->IsFilled() && _channel->GetAngleType() != AnalogIn::ANGLE_TYPE::NoAngleType)
		_channel->SetMinlevel(minLevel);
	_calibration->CreateKoeffTable();
	_filter->CreateFilterBuffer();
	_rate->CreateRateBuffer();
	UpdateDataToHW();
}

void ChAnalogIn::UpdateDataToHW()
{
}


void ChAnalogIn::UpdateHWToData()
{
	auto value = _channel->GetValue();

	// �������� ������ ����
	if(_channel->GetAngleType() != AnalogIn::ANGLE_TYPE::NoAngleType)
	{
		value += registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONDELTA)].reg->GetValueFloat();
	  	value *= registers_t[static_cast<UINT>(REGISTER_ID::rCORRECTIONSIGN)].reg->GetValueFloat();
		value -= (static_cast<int>(value / 360) * 360) - 360;
		if (value >= (360.0f + registers_t[static_cast<UINT>(REGISTER_ID::rDEADZONE)].reg->GetValueFloat()))
			value -= 360.0f;
	}
	registers_t[static_cast<UINT>(REGISTER_ID::rVALUE)].reg->SetValue(value);
	IOSTATE st;
	st.dword = _channel->GetState().dword;
	// ���� ������ ��� ���������
	_calibration->UpdateValue();
	//������ �������
	_filter->UpdateValue();
	// ������ ��������
	_rate->UpdateValue();
	// ��������� ������� �������, ���� ����, ���� ������������ ��������� FiltrReady
	st.iostFiltrNotReady = _filter->IsFilterReady() ? 0 : 1;
	registers_t[static_cast<UINT>(REGISTER_ID::rSTATE)].reg->SetValue(static_cast<UINT>(st.dword));
	// ����� ����������������� ������
	ResetCheckAlarm();
}

IChannel::CheckConfigurationResult ChAnalogIn::CheckConfiguration()
{
	auto res = IChannelIn::CheckConfiguration();
	if (res == IChannel::CheckConfigurationResult::OK)
	{
		res = _calibration->CheckConfiguration();
	}
	return res;
}


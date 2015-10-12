/*
 * ichannelcalibration.cpp
 *
 *  Created on: 14 июня 2015 г.
 *      Author: Youbrin_A
 */

#include "channelcalibration.h"

#include <string.h>

#include "ichannel.h"
#include "cregister.h"

ChannelCalibration::ChannelCalibration(IChannel* channel, bool isDeltaHRequired) : _channel(channel)
{
	_koeffTable = nullptr; // Коэффициенты для LINEAR
	_koeffCount = 0; // Длина таблицы (количество пар)
	_isCalibrated = false;
	_isMeasuring = false;
	_userPassport = nullptr;
	_userPassportCount = 0;
	_tarKoeffTable = nullptr;
	_tarKoeffCount = 0;
	_isDeltaHRequired = isDeltaHRequired;

}

bool ChannelCalibration::IsDeltaHRequired()
{
	return _channel->GetRegister(REGISTER_ID::rDELTA) != nullptr && _channel->GetRegister(REGISTER_ID::rDELTA)->IsFilled();
}

void ChannelCalibration::CreateClibrationRegisters() const
{
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rTARMIN)] = {REGISTER_ID::rTARMIN, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rTARMAX)] = {REGISTER_ID::rTARMAX, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rPASSPORT)] = {REGISTER_ID::rPASSPORT, rwConstant, rtArray, 4, 128, 1, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rRTCNUM)] = {REGISTER_ID::rRTCNUM, rwConstant, rtDec, 0, 22, 0, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rRTCVAL)] = {REGISTER_ID::rRTCVAL, rwConstant, rtDec, 0, 1, 1, false};
	if (_isDeltaHRequired)
		_channel->registers_t[static_cast<UINT>(REGISTER_ID::rDELTA)] = {REGISTER_ID::rDELTA, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
}

void ChannelCalibration::CreateKoeffTable(float**& kTable, UINT& kCount, const float* pasp, const UINT paspCount)
{
	if (paspCount < 4 || pasp == nullptr)
	{
		kTable = nullptr;
		kCount = 0;
	} else
	{
		kCount = (paspCount >> 1) - 1;
		kTable = new float*[kCount + 1];
		auto descending = (pasp[1] > pasp[3]);
		for(auto i = 0; i < kCount; i++)
		{
			UINT index = (descending ? (kCount - i - 1) : i);
			kTable[index] = new float[3];
			auto y1 = pasp[(i << 1)];
			auto x1 = pasp[(i << 1) + 1];
			auto y2 = pasp[((i+1) << 1)];
			auto x2 = pasp[((i+1) << 1) + 1];

			kTable[index][0] = descending ? x1 : x2;   // Диапазон
			kTable[index][1] = (y2 - y1)/(x2-x1);
			kTable[index][2] = y1 - kTable[index][1]*x1;
		}
		kTable[kCount] = nullptr;
	}
}

void ChannelCalibration::CreateKoeffTable()
{
	// Очистка предыдущий значений
	if (_koeffTable != nullptr && _koeffCount != 0)
	{
		for(auto i = 0; i < _koeffCount; i++)
			delete[] _koeffTable[i];
		delete[] _koeffTable;
		_koeffCount = 0;
	}


	// Проверка по паспорту или по настроенному каналу
	float* linTab = nullptr;
	UINT linSize = 0;
	if (_userPassport != nullptr && _userPassportCount >= 4)
	{
		_isCalibrated = true;
		linTab = _userPassport;
		linSize = _userPassportCount;
	}
	else
	{
		_isCalibrated = false;
		if (_channel->GetRegister(REGISTER_ID::rPASSPORT)->IsFilled())
		{
			linTab = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArray();
			linSize = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArraySize();
		}
	}
	CreateKoeffTable(_koeffTable, _koeffCount, linTab, linSize);
}

float ChannelCalibration::GetValueByKoeff(float deg) const
{
	if (_koeffCount==0 || _koeffTable==nullptr)
		return 0;
	if (_koeffCount == 1)
		return _koeffTable[0][1] * deg + _koeffTable[0][2];
	auto index = _koeffCount-1;
	for (UINT i = 0; i < _koeffCount; i++)
		if (deg < _koeffTable[i][0])
		{
			index = i;
			break;
		}
    if (index != -1)
		return _koeffTable[index][1] * deg + _koeffTable[index][2];
	else
		return 0;
}

float ChannelCalibration::GetTarPaspValue(float value)
{
	if (_tarKoeffCount==0 || _tarKoeffTable==nullptr)
	{
		UINT linSize = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArraySize();
		float* linTab = new float[linSize];
		for(UINT i = 0; i < linSize; i+=2)
		{
			linTab[i+1] = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetValueArray(i);
			linTab[i] = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetValueArray(i+1);
		}
		CreateKoeffTable(_tarKoeffTable, _tarKoeffCount, linTab, linSize);
	}
	if (_tarKoeffCount == 0)
		return 0;
	if (_tarKoeffCount == 1)
		return _tarKoeffTable[0][1] * value + _tarKoeffTable[0][2];
	int index = _tarKoeffCount-1;
	for (UINT i = 0; i < _tarKoeffCount; i++)
		if (value < _tarKoeffTable[i][0])
		{
			index = i;
			break;
		}
    if (index != -1)
		return _tarKoeffTable[index][1] * value + _tarKoeffTable[index][2];
	else
		return 0;
}


float ChannelCalibration::GetPoint(UINT index)
{
	if (!IsCalibrated())
	{
		if (index < _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArraySize())
		{
			return _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetValueArray(index);
		}
	}
	else
	{
		if (index < _userPassportCount)
		{
			return _userPassport[index];
		}
	}
	return 0;
}

UINT ChannelCalibration::GetPointsCount()
{
	if (!IsCalibrated())
		return _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArraySize();
	else
		return _userPassportCount;

}

void ChannelCalibration::UpdateValue()
{
	if (_isMeasuring)
	{
		_measuredValue += _channel->GetRegister(REGISTER_ID::rVALUE)->GetValueFloat();
		_currentMeasuringIndex++;
		if (_currentMeasuringIndex >= _measuringLength)
		{
			_isMeasuring = false;
			SetFlag(Flags::MeasurmentComplete);
		}
	}

}


void ChannelCalibration::StartMeasure()
{
	_isMeasuring = true;
	_measuredValue = 0;
	_currentMeasuringIndex = 0;
}


void ChannelCalibration::SetUserPassport(const float* userPasp, UINT count)
{
	if (_userPassport != nullptr)
		delete[] _userPassport;
	if (count >= 4 && userPasp != nullptr)
	{
		_userPassport = new float[count];
		_userPassportCount = count;
		memcpy(_userPassport, userPasp, sizeof(float) * count);
	}
	else
	{
		_userPassport = nullptr;
		_userPassportCount = 0;
	}
}

float ChannelCalibration::GetTarMin() const
{
	return _channel->GetRegister(REGISTER_ID::rTARMIN)->GetValueFloat();
}

float ChannelCalibration::GetTarMax() const
{
	return _channel->GetRegister(REGISTER_ID::rTARMAX)->GetValueFloat();
}


IChannel::CheckConfigurationResult ChannelCalibration::CheckConfiguration()
{
	if (_channel->GetRegister(REGISTER_ID::rPASSPORT)->IsFilled() &&
			(!_channel->GetRegister(REGISTER_ID::rTARMAX)->IsFilled() ||
			 !_channel->GetRegister(REGISTER_ID::rTARMIN)->IsFilled()))
	{
		return IChannel::CheckConfigurationResult::TarMinMaxNotFilled;
	}
	return IChannel::CheckConfigurationResult::OK;
}

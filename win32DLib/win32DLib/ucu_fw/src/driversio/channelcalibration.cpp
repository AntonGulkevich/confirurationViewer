/*
 * ichannelcalibration.cpp
 *
 *  Created on: 14 ���� 2015 �.
 *      Author: Youbrin_A
 */

#include "channelcalibration.h"
#include "ichannel.h"

ChannelCalibration::ChannelCalibration(IChannel* channel) : _channel(channel)
{
	koeffTable = nullptr; // ������������ ��� LINEAR
	koeffCount = 0; // ����� ������� (���������� ���)
}

void ChannelCalibration::CreateClibrationRegisters() const
{
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rTARMIN)] = {REGISTER_ID::rTARMIN, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rTARMAX)] = {REGISTER_ID::rTARMAX, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rPASSPORT)] = {REGISTER_ID::rPASSPORT, rwConstant, rtArray, 4, 128, 1, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rRTCNUM)] = {REGISTER_ID::rRTCNUM, rwConstant, rtDec, 0, 22, 0, false};
	_channel->registers_t[static_cast<UINT>(REGISTER_ID::rRTCVAL)] = {REGISTER_ID::rRTCVAL, rwConstant, rtDec, 0, 1, 1, false};
}

void ChannelCalibration::CreateKoeffTable()
{
	// ������� ���������� ��������
	if (koeffTable != nullptr && koeffCount != 0)
	{
		for(auto i = 0; i < koeffCount; i++)
			delete[] koeffTable[i];
		delete[] koeffTable;
		koeffCount = 0;
	}


	// �������� �� �������� ��� �� ������������ ������
	float* linTab = nullptr;
	UINT linSize = 0;
	// if () ���� ���� ��������� ������
	if (_channel->GetRegister(REGISTER_ID::rPASSPORT)->IsFilled())
	{
		linTab = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArray();
		linSize = _channel->GetRegister(REGISTER_ID::rPASSPORT)->GetArraySize();
	}

	if (linSize < 4 || linTab == nullptr)
	{
		return;
	}
	koeffCount = (linSize >> 1) - 1;
	koeffTable = new float*[koeffCount + 1];
	auto descending = (linTab[1] > linTab[3]);
	for(auto i = 0; i < koeffCount; i++)
	{
		UINT index = (descending ? (koeffCount - i - 1) : i);
		koeffTable[index] = new float[3];
		auto y1 = linTab[(i << 1)];
		auto x1 = linTab[(i << 1) + 1];
		auto y2 = linTab[((i+1) << 1)];
		auto x2 = linTab[((i+1) << 1) + 1];

		koeffTable[index][0] = descending ? x1 : x2;   // ��������
		koeffTable[index][1] = (y2 - y1)/(x2-x1);
		koeffTable[index][2] = y1 - koeffTable[index][1]*x1;
	}
	koeffTable[koeffCount] = nullptr;
}

float ChannelCalibration::GetValueByKoeff(float deg) const
{
	if (koeffCount==0 || koeffTable==nullptr)
		return 0;
	if (koeffCount == 1)
		return koeffTable[0][1] * deg + koeffTable[0][2];
	auto index = koeffCount-1;
	for (UINT i = 0; i < koeffCount; i++)
		if (deg < koeffTable[i][0])
		{
			index = i;
			break;
		}
    if (index != -1)
		return koeffTable[index][1] * deg + koeffTable[index][2];
	else
		return 0;
}

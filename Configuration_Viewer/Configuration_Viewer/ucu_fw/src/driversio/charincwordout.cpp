/*
 * charincin.cpp
 *
 *  Created on: 03 июн€ 2015 г.
 *      Author: Youbrin_A
 */

#include "charincwordout.h"

#include "../driversio/charincout.h"

#include "../driversiohw/arinctypes.h"
#include "../driversiohw/arincwordout.h"
#include "cregister.h"

ChArincWordOut::ChArincWordOut(CPattern* const pattern, ChArincOut* patternChannel, UINT number) : IChannelOut(pattern), _patternChannel(patternChannel)
{
	_channel = nullptr;
	_ioType = IOTYPES::ioArincWordOut;
	_number = number;
	_period = 20;

	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
		registers_t[i].id = REGISTER_ID::NULLID;

	registers_t[static_cast<UINT>(REGISTER_ID::rNAME)] = {REGISTER_ID::rNAME, rwConstant, rtString, 0.0f, 0.0f, 0.0f, false},
	registers_t[static_cast<UINT>(REGISTER_ID::rVALUE)] = {REGISTER_ID::rVALUE, rwVariable, rtFloat, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rSTATE)] = {REGISTER_ID::rSTATE, rwVariable, rtDec, 0.0f, 0xFFFF, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rMATRIX)] = {REGISTER_ID::rMATRIX, rwVariable, rtDec, 0.0f, 4, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rPERIODMAX)] = {REGISTER_ID::rPERIODMAX, rwConstant, rtFloat, 0.0f, 20000.0f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rPERIODMIN)] = {REGISTER_ID::rPERIODMIN, rwConstant, rtFloat, 0.0f, 20000.0f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rID)] = 	{REGISTER_ID::rID, rwConstant, rtDec, 0.0f, 3, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rGROUP)] = 	{REGISTER_ID::rGROUP, rwConstant, rtDec, 0.0f, 99.0f, 0.0f, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rSUBGROUP)] = 	{REGISTER_ID::rSUBGROUP, rwConstant, rtDec, 1.0f, 9.0f, 1.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rUNIT)] = 	{REGISTER_ID::rUNIT, rwConstant, rtDec, 0.0f, 17.0f, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rCONTROL)] = 	{REGISTER_ID::rCONTROL, rwVariable, rtDec, 0.0f, 1.0f, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rDELTA)] = 	{REGISTER_ID::rDELTA, rwConstant, rtDec, MIN_FLOAT, MAX_FLOAT, 0.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rBITSCOUNT)] = 	{REGISTER_ID::rBITSCOUNT, rwConstant, rtDec, 1, 22, 21.0f, false};
	registers_t[static_cast<UINT>(REGISTER_ID::rRANGE)] = 	{REGISTER_ID::rRANGE, rwConstant, rtFloat, MIN_FLOAT, MAX_FLOAT, (1 << 20), false};

	CreateRegisters();

}

void ChArincWordOut::AssignChannel(IOutputValue* ch)
{ _channel = static_cast<ArincWordOut*>(ch); }

void ChArincWordOut::InitRegisters()
{
	auto periodMin = GetRegister(REGISTER_ID::rPERIODMIN)->GetValueUInt();
	auto periodMax = GetRegister(REGISTER_ID::rPERIODMAX)->GetValueUInt();
	_period = (periodMax + periodMin) / 2;
	_channel->SetPeriod(_period);
	UpdateDataToHW();
}


void ChArincWordOut::UpdateDataToHW()
{
	auto value = GetRegister(REGISTER_ID::rVALUE)->GetValueUInt();
	Arinc::ARINC_STRUCT data;
	data.arincDef.label = _channel->GetLabel();

	if (GetRegister(REGISTER_ID::rID)->IsFilled())
	{
		data.arincId.data = (((value >> 10) + (GetRegister(REGISTER_ID::rDELTA)->GetValueUInt() >> 2)) & 0x7FFFF);
		data.arincId.id = GetRegister(REGISTER_ID::rID)->GetValueUInt();
	} else
	{
		data.arincDef.data = (((value >> 8) + (GetRegister(REGISTER_ID::rDELTA)->GetValueUInt())) & 0x1FFFFF);
	}

	if (GetRegister(REGISTER_ID::rMATRIX)->GetValueUInt() <= 3)
		data.arincDef.matrix = GetRegister(REGISTER_ID::rMATRIX)->GetValueUInt();
	else
		data.arincDef.matrix = ((value >> 29) & 3);
	data.arincDef.parity = (value >> 31) & 1;
	_channel->SetValue(static_cast<float>(data.dword >> 8));
	_channel->SetIsOnline(IsOnline());



}


void ChArincWordOut::UpdateHWToData()
{
	// —брос пользовательского отказа
	ResetCheckAlarm();

}

void ChArincWordOut::SetTableMode() const
{
	_channel->SetTableMode();
}

void ChArincWordOut::SetId(UINT id)
{
	_patternChannel->LabelToId(_number, id & 0x3);
	_number = _number | 0x4 | (id << 8);
}

bool ChArincWordOut::IsOnline()
{
	return GetRegister(REGISTER_ID::rCONTROL)->GetValueUInt() == 0;
}



#include "cpattern.h"

#include "../driversio/charincin.h"
#include "../driversio/charincout.h"
#include "../driversio/charincwordin.h"
#include "../driversio/charincwordout.h"
#include "../driversio/chpotentiometr.h"
#include "../driversio/chanalogin.h"
#include "../driversio/chnptout.h"
#include "../driversio/chsctout.h"
#include "../driversio/chlogicalin.h"
#include "../driversio/chlogicalout.h"
#include "../driversio/chdescreetein.h"
#include "../driversio/chdescreeteout.h"
#include "../driversio/chnotconnectedio.h"
#include "../driversiohw/arincwordin.h"
#include "../driversiohw/arincwordout.h"
#include "../driversiomanager.h"
#include "cprocessor.h"

CPattern::CPattern(DriversIOManager* driversIO, CConfiguration* config, UINT lSelfNum, bool lConstant)
{
	_config = config;
	_driversIO = driversIO;
	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
		registers_t[i].id = REGISTER_ID::NULLID;
	registers_t[static_cast<UINT>(REGISTER_ID::rNAME)] = {REGISTER_ID::rNAME, rwConstant, rtString, 0,0,0, true};
	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
	{
		if (registers_t[i].id != REGISTER_ID::NULLID)
			registers_t[i].reg = new CRegister();
		else
			registers_t[i].reg = nullptr;
	}

	deviceNum = 0xFF; 
	selfNum = lSelfNum; 
    constant = lConstant;
}

CPattern::~CPattern()
{
    for(UINT i = 0; i < ins.size(); i++)
        delete ins[i];
    for(UINT i = 0; i < outs.size(); i++)
        if (outs[i])
			delete outs[i];
       
}


CPattern::ADDIO_RESULT CPattern::AddIn(IOTYPES type, BYTE num, BYTE label)
{
	if (IndexOfIns(type, num) == -1)
	{
		switch(type)
		{
		case ioDigitalIn:
			ins.push_back(new ChArincIn(this, num));
			break;
		case ioAnalog:
			ins.push_back(new ChAnalogIn(this, num));
			break;
		case ioPotentiometr:
			ins.push_back(new ChPotentiometr(this, num));
			break;
		case ioRSIn:
			//ins.push_back(new ChRSIn());
			break;
		case ioCheck:
			ins.push_back(new ChDescreeteIn(this, num));
			break;
		case ioLogicalIn:
			ins.push_back(new ChLogicalIn(this, num));
			break;
		case ioArincWordIn:
		{
			auto i = IndexOfIns(ioDigitalIn, num);
			if (i != -1)
			{
				if (!static_cast<ChArincIn*>(ins[i])->IsLabelExist(label))
					static_cast<ChArincIn*>(ins[i])->AddLabel(label);
				else
					return ADDIO_RESULT::AlreadyExist;
			}else
				return ADDIO_RESULT::Error;
		}
			break;
		default:
			throw;
		}
		return ADDIO_RESULT::OK;
	} else
		return ADDIO_RESULT::AlreadyExist;
}

CPattern::ADDIO_RESULT CPattern::AddOut(IOTYPES type, BYTE num, BYTE label)
{
	if (IndexOfOuts(type, num) == -1)
	{
		switch(type)
		{
		case ioDigitalOut:
			outs.push_back(new ChArincOut(this, num));
			break;
		case ioNPT:
			outs.push_back(new ChNPTOut(this, num));
			break;
		case ioCommand:
			outs.push_back(new ChDescreeteOut(this, num));
			break;
		case ioSCT:
			outs.push_back(new ChSCTOut(this, num));
			break;
		case ioLogicalOut:
			outs.push_back(new ChLogicalOut(this, num));
			break;
		case ioRSOut:
			//out.push_back(new CRSOut());
			break;
		case ioArincWordOut:
		{
			auto i = IndexOfOuts(ioDigitalOut, num);
			if (i != -1)
			{
				if (!static_cast<ChArincOut*>(outs[i])->IsLabelExist(label))
					static_cast<ChArincOut*>(outs[i])->AddLabel(label);
				else
					return ADDIO_RESULT::AlreadyExist;
			}else
				return ADDIO_RESULT::Error;
		}
			break;
		default:
			throw;
		}
		return ADDIO_RESULT::OK;
	}
	else
		return ADDIO_RESULT::AlreadyExist;
}

CPattern::ADDIO_RESULT CPattern::AddNCIO(IOTYPES type, BYTE num)
{
	if (!IsNC(type, num))
		ncio.push_back(new ChNotConnectedIO(this, type, num));
	else 
		return ADDIO_RESULT::AlreadyExist;
	return ADDIO_RESULT::OK;
}

int CPattern::IndexOfIns(IOTYPES type, BYTE num)
{
	for(UINT i = 0; i < ins.size(); i++)
		if (ins[i]->IsNumberType(type, num))
			return i;
	return -1;
}

int CPattern::IndexOfOuts(IOTYPES type, BYTE num)
{
	for(UINT i = 0; i < outs.size(); i++)
		if (outs[i]->IsNumberType(type, num))
			return i;
	return -1;
}

CProcessor* CPattern::GetChannel(IOTYPES type, BYTE num)
{
	auto out = GetOutput(type, num);
	if (out != nullptr)
		return out->GetProcessor();
	return nullptr;
}

CProcessor* CPattern::GetChannelLabel(BYTE num, BYTE label) // Id пока не понятно как использовать
{
	auto out = GetOutput(ioDigitalOut, num);
	if (out != nullptr && static_cast<ChArincOut*>(out)->GetLabel(label) != nullptr)
		return static_cast<ChArincOut*>(out)->GetLabel(label)->GetProcessor();
	return nullptr;
}

IChannelIn* CPattern::GetInput(IOTYPES type, UINT num)
{
	auto res = IndexOfIns(type, num);
	if (res == -1)
		return nullptr;
	else
		if (res < static_cast<int>(ins.size()))
			return ins[res];
		else
			return nullptr;
}

IChannelOut* CPattern::GetOutput(IOTYPES type, UINT num)
{
	auto res = IndexOfOuts(type, num);
	if (res == -1)
		return nullptr;
	else
		if (res < static_cast<int>(outs.size()))
			return outs[res];
		else
			return nullptr;
}


bool CPattern::IsNC(IOTYPES type, BYTE num)
{
	for(UINT i = 0; i < ncio.size(); i++)
		if (ncio[i]->IsNumberType(type, num))
			return true;
	return false;
}

CPattern::VALIDIO_RESULT CPattern::ValidIO(IOTYPES type, UINT index)
{
	if (IsNC(type, index))
		return VALIDIO_RESULT::NC; // Если в(ы)ход не помечен как не подключенный, то удаление в(ы)хода
	if (IS_IN(type)) // Входы
	{
		if (GetInput(type, index) == nullptr)
			return VALIDIO_RESULT::NotExist;
		else if (GetInput(type, index)->IsConfigured())	// Если уже сконфигурирован, значит повторная конфигурация - ошибка
			return VALIDIO_RESULT::AlreadyConfigured;
	}
	else
	{
		if (GetOutput(type, index) == nullptr)
			return VALIDIO_RESULT::NotExist;
		else if (GetOutput(type, index)->IsConfigured())  // Если уже сконфигурирован, значит повторная конфигурация - ошибка
			return VALIDIO_RESULT::AlreadyConfigured;
	}
	return VALIDIO_RESULT::OK;
}

int CPattern::InitDevice()
{
	IOTYPES lType;
	BYTE lNum;
	BYTE lPat;
	UINT insize = ins.size();
	for(UINT i = 0; i < insize; i++)
	{
		if (_config->GetConnection(GetSelfNum(), ins[i]->GetType(), ins[i]->GetNumber(), lPat, lType, lNum) != -1)
		{
			if (ins[i]->GetType() == ioLogicalIn) // Логический соединяется с логическим выходом
				ins[i]->AssignChannel(reinterpret_cast<IInputValue*>(_config->GetPattern(lPat)->GetOutput(ioLogicalOut, lNum)));
			else
				ins[i]->AssignChannel(_driversIO->GetInput(lType, lNum));
			if (ins[i]->GetType() == ioDigitalIn) // Для ARINC соединяем отдельно каждую метку
				for(auto l = 0; l < 256; l++)
					if (static_cast<ChArincIn*>(ins[i])->IsLabelExist(l))
					{
						auto word = new ArincWordIn(_driversIO->GetArincIn(lNum), l);
						_driversIO->AddArincWord(word);
						static_cast<ChArincIn*>(ins[i])->GetLabel(l)->AssignChannel(word);
						static_cast<ChArincIn*>(ins[i])->GetLabel(l)->InitRegisters();
						ins.push_back(static_cast<ChArincIn*>(ins[i])->GetLabel(l)); // Добавление метки в общий список каналов
					}
			ins[i]->InitRegisters();

		}
	}
	UINT outsize = outs.size();
	for(UINT i = 0; i < outsize; i++)
	{
		if (_config->GetConnection(GetSelfNum(), outs[i]->GetType(), outs[i]->GetNumber(), lPat, lType, lNum) != -1)
		{
			if (outs[i]->GetType() != ioLogicalOut) // Логический выход никому ничего не должен
				outs[i]->AssignChannel(_driversIO->GetOutput(lType, lNum));
			if (outs[i]->GetType() == ioDigitalOut) // Сначала инициализируем метки
				for(auto l = 0; l < 256; l++)
					if (static_cast<ChArincOut*>(outs[i])->IsLabelExist(l))
					{
						auto word = new ArincWordOut(_driversIO->GetArincOut(lNum), l);
						_driversIO->AddArincWord(word);
						static_cast<ChArincOut*>(outs[i])->GetLabel(l)->AssignChannel(word);
						static_cast<ChArincOut*>(outs[i])->GetLabel(l)->InitRegisters();
						outs.push_back(static_cast<ChArincOut*>(outs[i])->GetLabel(l));
					}
			outs[i]->InitRegisters();

		}
	}
	return 0;
}

void CPattern::CompileDevice()
{
#ifndef _NO_COMPILIER
    for(UINT i = 0; i < outs.size(); i++)
    {
		if (!constant)
		{
			outs[i]->GetProcessor()->Compile();
		}
    }
#endif
}

void CPattern::InstanceDevice()
{
    for(UINT i = 0; i < outs.size(); i++)
    {
        outs[i]->GetProcessor()->Instance();
        outs[i]->GetProcessor()->Init();
    }
    
}

void CPattern::ClearDevice()
{
	for(UINT i = 0; i < ins.size(); i++)
		delete ins[i];
	for(UINT i = 0; i < outs.size(); i++)
		delete outs[i];
	ins.clear();
	outs.clear();
	// ReSharper disable once CppRedundantControlFlowJump
	return;
}

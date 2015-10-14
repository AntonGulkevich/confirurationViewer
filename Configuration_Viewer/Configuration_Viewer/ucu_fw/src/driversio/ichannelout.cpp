/*
 * ichannelin.cpp
 *
 *  Created on: 01 ���� 2015 �.
 *      Author: Youbrin_A
 */

#include "ichannelout.h"

#include "../application/cpattern.h"
#include "../application/cprocessor.h"
#include "../driversio/charincin.h"
#include "../driversio/charincout.h"


IChannelOut::IChannelOut(CPattern* const pattern): IChannel(pattern), _proc(new CProcessor(this))
{


}

IChannel* IChannelOut::GetPatternChannel(IOTYPES ioType, UINT num) const
{
	if (IS_IN(ioType))
		return _pattern->GetInput(ioType, num);
	else if (IS_OUT(ioType))
		return _pattern->GetOutput(ioType, num);
	return nullptr;
}

IChannel* IChannelOut::GetPatternChannelLabel(IOTYPES ioType, UINT num, BYTE label, BYTE id) const
{
	if (IS_IN(ioType))
		return reinterpret_cast<IChannel*>(static_cast<ChArincIn*>(_pattern->GetInput(ioType, num))->GetLabel(label));  // �������� ������ �� ID
	else if (IS_OUT(ioType))
		return reinterpret_cast<IChannel*>(static_cast<ChArincOut*>(_pattern->GetOutput(ioType, num))->GetLabel(label));  // �������� ������ �� ID;
	return nullptr;
}
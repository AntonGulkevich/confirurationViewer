/*
 * MenuAnalogParamsView.cpp
 *
 *  Created on: 11 сент. 2015 г.
 *      Author: Youbrin_A
 */

#include "menuanalogparamsview.h"

#include <stdio.h>

#include "../../application/cconfiguration.h"

#include "../../driversio/channelcalibration.h"
#include "../../driversio/ichannel.h"
#include "../../driversio/cregister.h"

MenuAnalogParamsView::MenuAnalogParamsView(IChannel* channel, const char* name, const BYTE points) : MenuItem(name, points), _channel(channel)
{
	_stage = STAGES::Header;

}

const char* MenuAnalogParamsView::GetElementName()
{
	_points = 0;
	switch(_stage)
	{
	case STAGES::Header:
		return MenuItem::GetElementName();
	case STAGES::Delay:
		sprintf(_headerBuffer, "%04d", _channel->GetRegister(REGISTER_ID::rDELAY)->GetValueInt());
		return _headerBuffer;
	case STAGES::Sign:
		sprintf(_headerBuffer, "%s", _channel->GetRegister(REGISTER_ID::rCORRECTIONSIGN)->GetValueInt() >= 0 ? "++++" : "----");
		return _headerBuffer;
	case STAGES::Delta:
		sprintf(_headerBuffer, "%04d", _channel->GetRegister(REGISTER_ID::rCORRECTIONDELTA)->GetValueInt());
		return _headerBuffer;
	case STAGES::Sync:
		sprintf(_headerBuffer, "C0%01d0", _channel->GetRegister(REGISTER_ID::rSYNCNUM)->GetValueInt());
		return _headerBuffer;
	}
	return "    ";
}

void MenuAnalogParamsView::Enter()
{
	switch(_stage)
	{
	case STAGES::Header:
		_stage = STAGES::Delay;
		break;
	case STAGES::Delay:
		_stage = STAGES::Sign;
		break;
	case STAGES::Sign:
		_stage = STAGES::Delta;
		break;
	case STAGES::Delta:
		_stage = STAGES::Sync;
		break;
	case STAGES::Sync:
		_stage = STAGES::Header;
		break;

	}
}

bool MenuAnalogParamsView::Exit()
{
	switch(_stage)
	{
	case STAGES::Header:
		return true;
		break;
	case STAGES::Delay:
	case STAGES::Sign:
	case STAGES::Delta:
	case STAGES::Sync:
		_stage = STAGES::Header;
		break;
	default:
		break;
	}
	return false;
}


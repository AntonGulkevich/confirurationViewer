/*
 * menuchannelaction.cpp
 *
 *  Created on: 07 сент. 2015 г.
 *      Author: Youbrin_A
 */

#include "menuchannelaction.h"
#include "../../driversio/ichannel.h"
#include "../../application/cconfiguration.h"
#include "../../drivers/display.h"
#include "../../driversio/channelcalibration.h"
#include "../../utilities/m_math.h"
#include "../../driversio/cregister.h"


MenuChannelAction::MenuChannelAction(IChannel* channel, ACTIONS action, const char* name, const BYTE points) : MenuItem(name, points), _channel(channel), _action(action)
{
	_isAcception = false;
	_isFlashing = false;
	_flashingState = 0;
}

void MenuChannelAction::ValueToStr(float value)
{
	auto group = CConfiguration::Groups[_channel->GetRegister(REGISTER_ID::rGROUP)->GetValueUInt()];
	MenuItem::ValueToStr(_valueBuff, _points, value, group.trunc, group.frac, group.sign);
}

const char* MenuChannelAction::GetElementName()
{
	if (_isActive)
	{
		switch(_action)
		{
		case ACTIONS::ShowValue:
		{
			auto value = _channel->GetRegister(REGISTER_ID::rVALUE)->GetValueFloat();
			ValueToStr(value);
			return _valueBuff;
		}
			break;
		case ACTIONS::ShowLinearValue:
		{
			auto value = _channel->GetChCalibration()->GetValueByKoeff(_channel->GetRegister(REGISTER_ID::rVALUE)->GetValueFloat());
			ValueToStr(value);
			return _valueBuff;
		}
			break;
		case ACTIONS::ResetTarirovka:
			if (_isFlashing)
			{
				if (_flashingState == 0)
				{
					_flashingState = 1;
					return "    ";
				}else
				{
					_flashingState = 0;
				}
			}
			return MenuItem::GetElementName();
			break;
		default:
			break;
		}

	}
	else
		return MenuItem::GetElementName();
	return "    ";
}

void MenuChannelAction::Enter()
{
	switch(_action)
	{
	case ACTIONS::ShowLinearValue:
	case ACTIONS::ShowValue:
		_isActive = true;
		break;
	case ACTIONS::ResetTarirovka:
		if(!_isAcception)
		{
			_isAcception = true;
		}
		break;
	default:
		break;
	}
}

void MenuChannelAction::LongEnter()
{
	switch(_action)
	{
	case ACTIONS::ShowLinearValue:
	case ACTIONS::ShowValue:
		break;
	case ACTIONS::ResetTarirovka:
		if (_isAcception)
		{
			_isAcception = false;
			_channel->GetChCalibration()->SetUserPassport(nullptr, 0);
			for(;;);
		}
		break;
	default:
		break;
	}
}

bool MenuChannelAction::Exit()
{
	switch(_action)
	{
	case ACTIONS::ShowLinearValue:
	case ACTIONS::ShowValue:
		if (_isActive)
		{
			_isActive = false;
		}
	case ACTIONS::ResetTarirovka:
		if (_isActive)
		{
			if (_isAcception)
				_isAcception = false;
			else
				_isActive = false;
		}
		break;
	default:
		break;
	}
	return false;
}



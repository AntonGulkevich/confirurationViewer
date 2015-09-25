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
#include "../../utilities/m_math.h"

MenuChannelAction::MenuChannelAction(IChannel* channel, ACTIONS action, const char* name, const BYTE points) : MenuItem(name, points), _channel(channel), _action(action)
{

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
	default:
		break;
	}
	return false;
}



/*
 * menupassportview.cpp
 *
 *  Created on: 11 ñåíò. 2015 ã.
 *      Author: Youbrin_A
 */

#include "menupassportview.h"

#include <stdio.h>

#include "../../application/cconfiguration.h"

#include "../../driversio/channelcalibration.h"
#include "../../driversio/ichannel.h"

MenuPassportView::MenuPassportView(IChannel* channel, const char* name, const BYTE points) : MenuItem(name, points), _channel(channel)
{
	_stage = STAGES::Header;
	_pointNumber = 0;

}

const char* MenuPassportView::GetElementName()
{
	_points = 0;
	switch(_stage)
	{
	case STAGES::Header:
		return MenuItem::GetElementName();
	case STAGES::Start:
		if (_channel->GetChCalibration()->IsCalibrated())
			return "PEA_";
		else
			return "ÑÐÍ_";
	case STAGES::ViewHeader:
		sprintf(_headerBuffer, "%s_%02d", (_pointNumber & 1) == 1 ? "Y" : "C", (_pointNumber / 2) + 1);
		return _headerBuffer;
	case STAGES::ViewVal:
		if ((_pointNumber & 1) == 1)
		{
			ValueToStr(_headerBuffer, _points, _channel->GetChCalibration()->GetPoint(_pointNumber), 3, 1, true);
		} else
		{
			auto group = CConfiguration::Groups[_channel->GetRegister(REGISTER_ID::rGROUP)->GetValueUInt()];
			ValueToStr(_headerBuffer, _points, _channel->GetChCalibration()->GetPoint(_pointNumber), group.trunc, group.frac, group.sign);
		}
		return _headerBuffer;
	}
	return "    ";
}

void MenuPassportView::Enter()
{
	switch(_stage)
	{
	case STAGES::Header:
		_stage = STAGES::Start;
		_pointNumber = 0;
		break;
	case STAGES::Start:
		_stage = STAGES::ViewHeader;
		break;
	case STAGES::ViewHeader:
		_stage = STAGES::ViewVal;
		break;
	case STAGES::ViewVal:
		_pointNumber++;
		if (_pointNumber >= (int)_channel->GetChCalibration()->GetPointsCount())
			_stage = STAGES::Header;
		else
			_stage = STAGES::ViewHeader;
		break;

	}
}

bool MenuPassportView::Exit()
{
	switch(_stage)
	{
	case STAGES::Header:
		return true;
	case STAGES::Start:
	case STAGES::ViewHeader:
		_stage = STAGES::Header;
		break;
	case STAGES::ViewVal:
		_stage = STAGES::ViewHeader;
		break;
	}
	return false;
}


void MenuPassportView::Prev()
{
	switch(_stage)
	{
	case STAGES::Header:
	case STAGES::Start:
	case STAGES::ViewVal:
		break;
	case STAGES::ViewHeader:
		_pointNumber--;
		if (_pointNumber < 0)
			_pointNumber= 0;
		break;
	}
}


void MenuPassportView::Next()
{
	switch(_stage)
	{
	case STAGES::Header:
	case STAGES::Start:
	case STAGES::ViewVal:
		break;
	case STAGES::ViewHeader:
		_pointNumber++;
		if (_pointNumber >= (int)_channel->GetChCalibration()->GetPointsCount())
			_pointNumber= (int)_channel->GetChCalibration()->GetPointsCount() - 1;
		break;
	}
}
